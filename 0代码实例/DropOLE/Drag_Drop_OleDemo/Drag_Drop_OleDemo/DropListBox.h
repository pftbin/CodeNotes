#include "OleDropTargetEx.h"

#ifndef AFX_DROPLISTBOX_H_
#define AFX_DROPLISTBOX_H_

//CDropListBox
class CDropListBox : public CListBox
{
	DECLARE_DYNAMIC(CDropListBox)
public:
	CDropListBox();
public:
	virtual BOOL Register();
	virtual ~CDropListBox();
	BOOL m_bDraging;

protected:
	COleDropTargetEx m_dropEx;
	afx_msg LRESULT OnDrop(WPARAM pDropInfoClass, LPARAM lParm);
	afx_msg LRESULT OnDropEx(WPARAM pDropInfoClass, LPARAM lParm);
	afx_msg LRESULT OnDragOver(WPARAM pDropInfoClass,LPARAM lParm);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

#endif
