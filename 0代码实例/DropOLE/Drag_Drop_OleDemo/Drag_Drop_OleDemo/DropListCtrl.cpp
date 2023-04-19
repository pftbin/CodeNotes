// DropListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DropListCtrl.h"

#pragma comment(lib,"Shlwapi.lib")
#include <Shlwapi.h>              //Get FileSize API

// CDropListCtrl

IMPLEMENT_DYNAMIC(CDropListCtrl, CListCtrl)

CDropListCtrl::CDropListCtrl()
{
	m_bDraging = FALSE;
}

CDropListCtrl::~CDropListCtrl()
{
}


BEGIN_MESSAGE_MAP(CDropListCtrl, CListCtrl)
	ON_MESSAGE(DROPM_DRAGOVER,OnDragOver)
	ON_MESSAGE(DROPM_DROPEX,OnDropEx)
	ON_MESSAGE(DROPM_DROP,OnDrop)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindragFilelist) //DragOut is File
END_MESSAGE_MAP()



// CDropListCtrl message handlers
BOOL CDropListCtrl::Register()
{
	return m_dropEx.Register( this );
}

LRESULT CDropListCtrl::OnDragOver(WPARAM pDropInfoClass, LPARAM lParm)
{
	COleDropInfo* pInfo = (COleDropInfo* )pDropInfoClass;
	ASSERT(pInfo->IsKindOf(RUNTIME_CLASS(COleDropInfo)));

	if( pInfo->m_pDataObject->IsDataAvailable( CF_HDROP ) )
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

LRESULT CDropListCtrl::OnDropEx(WPARAM pDropInfoClass, LPARAM lParm)
{
	return (DROPEFFECT)-1;
}

LRESULT CDropListCtrl::OnDrop(WPARAM pDropInfoClass, LPARAM lParm)
{
	COleDropInfo* pInfo = (COleDropInfo* )pDropInfoClass;
	ASSERT(pInfo->IsKindOf(RUNTIME_CLASS(COleDropInfo)));

	COleDataObject* pDataObject = pInfo->m_pDataObject;

	HGLOBAL     hg;
	HDROP       hdrop;
	UINT        uNumFiles;

	TCHAR       szNextFile [MAX_PATH];
	SHFILEINFO  rFileInfo;
	LVFINDINFO  rlvFind = { LVFI_STRING, szNextFile };
	LVITEM      rItem;
	int         nIndex = GetItemCount();
	HANDLE      hFind;
	WIN32_FIND_DATA rFind;
	TCHAR       szFileLen [64];

	// Get the HDROP data from the data object.
	hg = pDataObject->GetGlobalData ( CF_HDROP ); 
	if ( NULL == hg )
	{
		return FALSE;
	}

	hdrop = (HDROP) GlobalLock ( hg );
	if ( NULL == hdrop )
	{
		GlobalUnlock ( hg );
		return FALSE;
	}

	// Get File Count 
	uNumFiles = DragQueryFile ( hdrop, -1, NULL, 0 );

	for ( UINT uFile = 0; uFile < uNumFiles; uFile++ )
	{
		if ( DragQueryFile ( hdrop, uFile, szNextFile, MAX_PATH ) > 0 )
		{
			if ( -1 != FindItem ( &rlvFind, -1 ))
				continue;

			//Get FileInfo
			SHGetFileInfo ( szNextFile, 0, &rFileInfo, sizeof(rFileInfo),
				SHGFI_SYSICONINDEX | SHGFI_ATTRIBUTES |
				SHGFI_TYPENAME );

			// Set up an LVITEM for the file we're about to insert.
			ZeroMemory ( &rItem, sizeof(LVITEM) );
			rItem.mask    = LVIF_IMAGE | LVIF_TEXT;
			rItem.iItem   = nIndex;
			rItem.pszText = szNextFile;
			rItem.iImage  = rFileInfo.iIcon;

			//File Attribute is Hide,Set ShowStyle in ListCtrl
			if ( rFileInfo.dwAttributes & SFGAO_GHOSTED )
			{
				rItem.mask |= LVIF_STATE;
				rItem.state = rItem.stateMask = LVIS_CUT;
			}

			// Insert it into the list!
			InsertItem ( &rItem );

			// Set column 1 to the file's type description.
			SetItemText ( nIndex, 1, rFileInfo.szTypeName );

			// Get the file size, show in column 2.
			hFind = FindFirstFile ( szNextFile, &rFind );
			if ( INVALID_HANDLE_VALUE != hFind )
			{
				StrFormatByteSize ( rFind.nFileSizeLow, szFileLen, 64 );
				FindClose ( hFind );
			}
			SetItemText ( nIndex, 2, szFileLen );

			nIndex++;
		}
	}   // end for

	GlobalUnlock ( hg );
	return TRUE;
}

void CDropListCtrl::OnBegindragFilelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW*    pNMLV = (NMLISTVIEW*) pNMHDR;
	COleDataSource datasrc;
	HGLOBAL        hgDrop;
	DROPFILES*     pDrop;
	CStringList    lsDraggedFiles;
	POSITION       pos;
	int            nSelItem;
	CString        sFile;
	UINT           uBuffSize = 0;
	TCHAR*         pszBuff;
	FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	*pResult = 0;   // return value ignored

	// For every selected item in the list, put the filename into lsDraggedFiles.

	pos = GetFirstSelectedItemPosition();

	while ( NULL != pos )
	{
		nSelItem = GetNextSelectedItem ( pos );
		sFile = GetItemText ( nSelItem, 0 );

		lsDraggedFiles.AddTail ( sFile );

		// Calculate the # of chars required to hold this string.

		uBuffSize += lstrlen ( sFile ) + 1;
	}

	// Add 1 extra for the final null char, and the size of the DROPFILES struct.

	uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);

	// Allocate memory from the heap for the DROPFILES struct.

	hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, uBuffSize );

	if ( NULL == hgDrop )
		return;

	pDrop = (DROPFILES*) GlobalLock ( hgDrop );

	if ( NULL == pDrop )
	{
		GlobalFree ( hgDrop );
		return;
	}

	// Fill in the DROPFILES struct.

	pDrop->pFiles = sizeof(DROPFILES);

#ifdef _UNICODE
	// If we're compiling for Unicode, set the Unicode flag in the struct to
	// indicate it contains Unicode strings.

	pDrop->fWide = TRUE;
#endif

	// Copy all the filenames into memory after the end of the DROPFILES struct.

	pos = lsDraggedFiles.GetHeadPosition();
	pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));

	while ( NULL != pos )
	{
		lstrcpy ( pszBuff, (LPCTSTR) lsDraggedFiles.GetNext ( pos ) );
		pszBuff = 1 + _tcschr ( pszBuff, '\0' );
	}

	GlobalUnlock ( hgDrop );

	// Put the data in the data source.

	datasrc.CacheGlobalData ( CF_HDROP, hgDrop, &etc );

	// Start the drag 'n' drop!

	DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY | DROPEFFECT_MOVE );

}
