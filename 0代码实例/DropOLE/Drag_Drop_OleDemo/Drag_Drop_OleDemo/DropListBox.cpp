// DropListBox.cpp : implementation file


#include "stdafx.h"
#include "DropListBox.h"


// CDropListBox

IMPLEMENT_DYNAMIC(CDropListBox, CListBox)

CDropListBox::CDropListBox()
{
	m_bDraging = FALSE;
}

CDropListBox::~CDropListBox()
{
}


BEGIN_MESSAGE_MAP(CDropListBox, CListBox)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(DROPM_DRAGOVER,OnDragOver)
	ON_MESSAGE(DROPM_DROPEX,OnDropEx)
	ON_MESSAGE(DROPM_DROP,OnDrop)
END_MESSAGE_MAP()

// CDropListBox message handlers
BOOL CDropListBox::Register()
{
	return m_dropEx.Register( this );
}

LRESULT CDropListBox::OnDragOver(WPARAM pDropInfoClass, LPARAM lParm)
{
	COleDropInfo* pInfo = (COleDropInfo* )pDropInfoClass;
	ASSERT(pInfo->IsKindOf(RUNTIME_CLASS(COleDropInfo)));

	if( pInfo->m_pDataObject->IsDataAvailable( CF_TEXT ) )
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

LRESULT CDropListBox::OnDropEx(WPARAM pDropInfoClass, LPARAM lParm)
{
	return (DROPEFFECT)-1;
}

LRESULT CDropListBox::OnDrop(WPARAM pDropInfoClass, LPARAM lParm)
{
	COleDropInfo* pInfo = (COleDropInfo* )pDropInfoClass;
	ASSERT(pInfo->IsKindOf(RUNTIME_CLASS(COleDropInfo)));

	if( pInfo->m_pDataObject->IsDataAvailable( CF_TEXT ) )
	{
		HGLOBAL hMem = pInfo->m_pDataObject->GetGlobalData( CF_TEXT );
		char* lp = (char *)GlobalLock((HGLOBAL) hMem);//lock source
		if (lp != NULL)
		{
			CString strText;
//			strText = lp;
			strText.Format(_T("%S"),lp);

			// if exist,delete
			int nIndex = 0;
			while ((nIndex=FindString(nIndex, strText)) != LB_ERR)
			{
				DeleteString( nIndex );
			}

			AddString(strText);

		}
		GlobalUnlock( hMem );//unlock source
		return TRUE;
	}
	else
		return FALSE;
}

void CDropListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonDown(nFlags, point);

	Sleep(250);
	MSG msg;
	::PeekMessage(&msg,GetSafeHwnd(),WM_LBUTTONUP,WM_LBUTTONUP,PM_NOREMOVE);
	if( msg.message==WM_LBUTTONUP )
		return;

	CString sContens;
	int nIndex = GetCurSel();
	if(nIndex == -1) return;
	GetText(nIndex,sContens);

	int len = sContens.GetLength();
	if(len<1) return;

	HGLOBAL hData = GlobalAlloc(GHND|GMEM_SHARE, len+1 );
	char* pS = (char*)GlobalLock( hData );

	//Unicode
	char szTarget[255] = {0};
	USES_CONVERSION;
	char* pTemp = T2A(sContens);
	strcpy(szTarget,pTemp);
	strcpy(pS,szTarget);

	//muti_byte
//		memcpy( pS, (LPCTSTR)sContens, sContens.GetLength()+1 );

	GlobalUnlock( hData );
	m_bDraging = TRUE;	//开始拖动
	COleDataSource source;
	source.CacheGlobalData( CF_TEXT, hData );
	source.DoDragDrop( DROPEFFECT_COPY );
	GlobalFree( hData );
	m_bDraging = FALSE;	//完成拖动
}