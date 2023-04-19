#include "OleDropTargetEx.h"

#ifndef AFX_DROPBUTTON_H_
#define AFX_DROPBUTTON_H_

//CDropButton
class CDropButton : public CButton
{
	DECLARE_DYNAMIC(CDropButton)
public:
	CDropButton();
public:
	virtual BOOL Register();
	virtual ~CDropButton();

protected:
	COleDropTargetEx m_dropEx;
	afx_msg LRESULT OnDrop(WPARAM pDropInfoClass, LPARAM lParm);
	afx_msg LRESULT OnDropEx(WPARAM pDropInfoClass, LPARAM lParm);
	afx_msg LRESULT OnDragOver(WPARAM pDropInfoClass,LPARAM lParm);

	DECLARE_MESSAGE_MAP()
};

#endif 
