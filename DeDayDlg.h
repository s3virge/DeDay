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

private:
	// путь к папке которую будем удалять
	CString m_csPath;
	CMonthCalCtrl m_MonthCalCtrl;
	BOOL m_bDeleteOnDrive;
	BOOL m_bKillWindows;

private:	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnBrows();
	afx_msg void OnEnChangePath();	
	afx_msg void OnBnClickedDeleteAllOnDrive();	
	afx_msg void OnBnClickedKillwindows();
	afx_msg void OnBnClickedCancel();

	void InitDriveLetterComboBox();
};
