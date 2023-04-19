// RuncmdDlg.h : header file
//

#if !defined(AFX_RUNCMDDLG_H__3B5C27BA_3112_4E46_BC33_794C75B5F6AD__INCLUDED_)
#define AFX_RUNCMDDLG_H__3B5C27BA_3112_4E46_BC33_794C75B5F6AD__INCLUDED_

#include "MAPHYPERLINK.H"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRuncmdDlg dialog

class CRuncmdDlg : public CDialog
{
// Construction
public:

	CRuncmdDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRuncmdDlg)
	enum { IDD = IDD_RUNCMD_DIALOG };
	CMapHyperLink	m_MapHyperLink;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuncmdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRuncmdDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RUNCMDDLG_H__3B5C27BA_3112_4E46_BC33_794C75B5F6AD__INCLUDED_)
