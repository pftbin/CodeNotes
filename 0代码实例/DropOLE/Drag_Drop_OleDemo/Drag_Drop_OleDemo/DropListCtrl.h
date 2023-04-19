#pragma once
#include "OleDropTargetEx.h"

#ifndef AFX_DROPLISTCTRL_H_
#define AFX_DROPLISTCTRL_H_

// CDropListCtrl
class CDropListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CDropListCtrl)

public:
	virtual BOOL Register();
	CDropListCtrl();
	virtual ~CDropListCtrl();
	BOOL m_bDraging;

	// Generated message map functions
protected:
	COleDropTargetEx m_dropEx;
	afx_msg LRESULT OnDrop(WPARAM pDropInfoClass, LPARAM lParm);
	afx_msg LRESULT OnDropEx(WPARAM pDropInfoClass, LPARAM lParm);
	afx_msg LRESULT OnDragOver(WPARAM pDropInfoClass,LPARAM lParm);
	afx_msg void	OnBegindragFilelist(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	DECLARE_MESSAGE_MAP()
};

#endif

