
// BurningToolGOSDCreatorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "C_GOSDCreatorDlg.h"
#include "C_GOSDParameterDlg.h"
#include "C_GOSDDataMgr.h"


// CBurningToolGOSDCreatorDlg dialog
class CBurningToolGOSDCreatorDlg : public CDialog
{
// Construction
public:
	CBurningToolGOSDCreatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BURNINGTOOLGOSDCREATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUpdateParameter( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

	void InitTabCtrlItemDlg();
private:
	CTabCtrl m_TabCtrl;
	C_GOSDDataMgr       m_GOSDMgr;
	C_GOSDCreatorDlg	m_CreatorDlg;
	C_GOSDParameterDlg	m_ParameterDlg;
	
	int m_xMin;
	int m_yMin;
	int m_nWidth;
	int m_nHeight;
};
