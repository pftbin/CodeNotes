#include "OleDropTargetEx.h"

#ifndef AFX_DROPEDIT_H_
#define AFX_DROPEDIT_H_

//CDropEdit
class CDropEdit : public CEdit
{
	DECLARE_DYNAMIC(CDropEdit)
public:
	CDropEdit();
public:
	BOOL m_bDraging;
	virtual BOOL Register();
	virtual ~CDropEdit();
protected:
	COleDropTargetEx m_dropEx;
	afx_msg LRESULT OnDrop(WPARAM pDropInfoClass, LPARAM lParm);
	afx_msg LRESULT OnDropEx(WPARAM pDropInfoClass, LPARAM lParm);
	afx_msg LRESULT OnDragOver(WPARAM pDropInfoClass,LPARAM lParm);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

#endif 
