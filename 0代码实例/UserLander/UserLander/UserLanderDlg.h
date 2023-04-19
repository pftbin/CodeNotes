
// UserLanderDlg.h : header file
//

#pragma once
#include "MySQlite3.h"

// CUserLanderDlg dialog
class CUserLanderDlg : public CDialog
{
// Construction
public:
	CUserLanderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_USERLANDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonReg();
	afx_msg void OnBnClickedButtonLogin();
private:
	MySQlite3 myDataBase;
public:
	CString m_strUserName;
	CString m_strPassWord;
	void OpenClient();
};
