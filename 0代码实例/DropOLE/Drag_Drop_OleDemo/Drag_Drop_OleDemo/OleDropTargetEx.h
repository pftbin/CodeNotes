// OleDropTargetEx.h: interface for the COleDropTargetEx class.

#ifndef AFX_OLEDROPTARGETEX_H_
#define AFX_OLEDROPTARGETEX_H_

#include "H_USER_Msg.h"
//Drop info class
class COleDropInfo : public CObject
{
public:
	COleDropInfo();
	COleDropInfo(COleDataObject* pDataObject,DWORD dwKeyState,
				 DROPEFFECT dropEffect,DROPEFFECT dropEffectList,CPoint point );
	virtual ~COleDropInfo();

public:
	//COleDropTargetEx member functions params
	COleDataObject*		m_pDataObject;
	DWORD				m_dwKeyState;
	DROPEFFECT			m_dropEffect;
	DROPEFFECT			m_dropEffectList;
	CPoint				m_point;

	DECLARE_DYNAMIC( COleDropInfo );
};

//COleDropTargetEx class 
class COleDropTargetEx : public COleDropTarget
{
public:
	COleDropTargetEx();
	virtual ~COleDropTargetEx();

protected:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
			DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
			DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
			DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject,
			DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
};

#endif 
