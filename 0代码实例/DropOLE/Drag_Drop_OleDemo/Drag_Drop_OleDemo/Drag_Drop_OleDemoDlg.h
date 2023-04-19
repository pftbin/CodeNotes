
// Drag_Drop_OleDemoDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "OleDropTargetEx.h"
#include "DropEdit.h"
#include "DropButton.h"
#include "DropListBox.h"
#include "DropListCtrl.h"


// CDrag_Drop_OleDemoDlg dialog
class CDrag_Drop_OleDemoDlg : public CDialog
{
// Construction
public:
	CDrag_Drop_OleDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DRAG_DROP_OLEDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CDropEdit		m_EditSource;
	CDropEdit		m_EditDrop;
	CDropButton		m_ButtonDrop;
	CDropListBox	m_ListBoxDrop;
	CDropListCtrl	m_listCtrl;
};
