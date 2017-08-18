// DeDay.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

//#include <string>
//using namespace std;


// CDeDayApp:
// See DeDay.cpp for the implementation of this class
//

class CDeDayApp : public CWinApp
{
public:
	CDeDayApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

	//bool InspectDeleteData();
	//bool DeleteFolder();
	//bool PerformATask(void);	
	//void ReadRegDeleteDay(int& DelDay, int& DelWeek, int& Month);
	//void GetCurrentDay(int& iDay, int& iDayOfWeek, int& iMonth);
	//bool CreateDiskpartScript(wchar_t[], wchar_t[], wchar_t[]);
	//bool CreateBatFile(wchar_t[], wchar_t[], wchar_t[], wchar_t[], wchar_t[]);
	//void KeyBoardLedBlink(void);
	//bool AddToAutorun(void);
	//bool IfKillWindows(void);
	//bool IfDeleteDiskD(void);
	//void DoKillWindows(void);
};

extern CDeDayApp theApp;
//extern CDeleteDay delDay;