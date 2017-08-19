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
};

extern CDeDayApp theApp;