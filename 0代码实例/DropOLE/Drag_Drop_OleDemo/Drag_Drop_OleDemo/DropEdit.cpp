// DropEdit.cpp : implementation file
//

#include "stdafx.h"
#include "DropEdit.h"


// CDropEdit

IMPLEMENT_DYNAMIC(CDropEdit, CEdit)

CDropEdit::CDropEdit()
{
	m_bDraging = FALSE;
}

CDropEdit::~CDropEdit()
{
}


BEGIN_MESSAGE_MAP(CDropEdit, CEdit)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(DROPM_DRAGOVER,OnDragOver)
	ON_MESSAGE(DROPM_DROPEX,OnDropEx)
	ON_MESSAGE(DROPM_DROP,OnDrop)
END_MESSAGE_MAP()

// CDropEdit message handlers

BOOL CDropEdit::Register()
{
	return m_dropEx.Register( this );
}

LRESULT CDropEdit::OnDragOver(WPARAM pDropInfoClass, LPARAM lParm)
{
	COleDropInfo* pInfo = (COleDropInfo* )pDropInfoClass;
	ASSERT(pInfo->IsKindOf(RUNTIME_CLASS(COleDropInfo)));

	if( pInfo->m_pDataObject->IsDataAvailable( CF_TEXT ) )
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

LRESULT CDropEdit::OnDropEx(WPARAM pDropInfoClass, LPARAM lParm)
{
	return (DROPEFFECT)-1;
}

LRESULT CDropEdit::OnDrop(WPARAM pDropInfoClass, LPARAM lParm)
{
	COleDropInfo* pInfo = (COleDropInfo* )pDropInfoClass;
	ASSERT(pInfo->IsKindOf(RUNTIME_CLASS(COleDropInfo)));

	if( pInfo->m_pDataObject->IsDataAvailable( CF_TEXT ) )
	{
		HGLOBAL hMem = pInfo->m_pDataObject->GetGlobalData( CF_TEXT );
		char* lp = (char*)GlobalLock((HGLOBAL) hMem);//lock source
		if (lp != NULL)
		{
			CString strText;
//			strText = lp;
			strText.Format(_T("%S"),lp);
			SetWindowText(strText);
		}
		GlobalUnlock( hMem );//unlock source
		return TRUE;
	}
	else
		return FALSE;
}

void CDropEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonDown(nFlags, point);
	Sleep(250);
	MSG msg;
	::PeekMessage(&msg,GetSafeHwnd(),WM_LBUTTONUP,WM_LBUTTONUP,PM_NOREMOVE);
	if( msg.message==WM_LBUTTONUP )
		return;

	CString sContens;
	GetWindowText( sContens );
	int len = sContens.GetLength();
	if(len<1) return;

	HGLOBAL hData = GlobalAlloc(GHND|GMEM_SHARE, len+1 );//1.分配全局内存
	char* pS = (char*)GlobalLock( hData );//2.锁定内存块返回头指针

	//3.为内存块赋值
	//Unicode
	char szTarget[255] = {0};
	USES_CONVERSION;
	char* pTemp = T2A(sContens);
	strcpy(szTarget,pTemp);
	strcpy(pS,szTarget);
	//muti_byte
//	memcpy( pS, (LPCTSTR)sContens, sContens.GetLength()+1 );

	GlobalUnlock( hData ); //4.解锁内存块

	m_bDraging = TRUE;	//开始拖动
	COleDataSource source;
	source.CacheGlobalData( CF_TEXT, hData );//5.指定数据在传输中的格式
	source.DoDragDrop( DROPEFFECT_COPY ); //6.直到OnDrop执行完毕返回
	GlobalFree( hData );
	m_bDraging = FALSE;	//完成拖动
}


