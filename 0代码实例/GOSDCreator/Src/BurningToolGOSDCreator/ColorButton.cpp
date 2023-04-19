//***************************************************************************
//
// AUTHOR:  James White (feel free to remove or otherwise mangle any part)
//
//***************************************************************************
#include "stdafx.h"
#include "ColorButton.h"

//***********************************************************************
//**                         MFC Debug Symbols                         **
//***********************************************************************
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//***********************************************************************
//**                            DDX Method                            **
//***********************************************************************

void AFXAPI DDX_ColorButton(CDataExchange *pDX, int nIDC, COLORREF& crColour)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT (hWndCtrl != NULL);                
    
    CColorButton* pColourButton = (CColorButton*) CWnd::FromHandle(hWndCtrl);
    if (pDX->m_bSaveAndValidate)
    {
		crColour = pColourButton->Color;
    }
    else // initializing
    {
		pColourButton->Color = crColour;
    }
}

//***********************************************************************
//**                             Constants                             **
//***********************************************************************
const int g_ciArrowSizeX = 4 ;
const int g_ciArrowSizeY = 2 ;

//***********************************************************************
//**                            MFC Macros                            **
//***********************************************************************
IMPLEMENT_DYNCREATE(CColorButton, CButton)

//***********************************************************************
// Method:	CColorButton::CColorButton(void)
// Notes:	Default Constructor.
//***********************************************************************
CColorButton::CColorButton(void):
	_Inherited(),
 	m_Color(CLR_DEFAULT),
// 	m_DefaultColor(::GetSysColor(COLOR_APPWORKSPACE)),
 //	m_Color(RGB( 0, 16, 129 )),
 	m_DefaultColor(RGB( 0, 16, 129 )),
	m_strDefaultText(_T("Default")),
	m_strCustomText(_T("Others...")),
	m_bPopupActive(FALSE),
	m_bTrackSelection(FALSE)
{
	m_bIsPressed		= FALSE;
	m_bIsFocused		= FALSE;
	m_bIsDisabled		= FALSE;
	m_bMouseOnButton	= FALSE;
	m_bAlwaysTrack = TRUE;
	m_bIsFlat = TRUE;
	m_bDrawBorder = FALSE; 
	m_ptImageOrg.x = 3;
	m_ptImageOrg.y = 3;
	m_byAlign = ST_ALIGN_HORIZ;
	m_bIsCheckBox = FALSE; 
	m_bSelected = false;
	SetPressedStyle(BTNST_PRESSED_LEFTRIGHT, FALSE);
}

//***********************************************************************
// Method:	CColorButton::~CColorButton(void)
// Notes:	Destructor.
//***********************************************************************
CColorButton::~CColorButton(void)
{
}

//***********************************************************************
// Method:	CColorButton::GetColor()
// Notes:	None.
//***********************************************************************
COLORREF CColorButton::GetColor(void) const
{
	return m_Color;
}


//***********************************************************************
// Method:	CColorButton::SetColor()
// Notes:	None.
//***********************************************************************
void CColorButton::SetColor(COLORREF Color)
{
	m_Color = Color;

	if (::IsWindow(m_hWnd)) 
        RedrawWindow();
}


//***********************************************************************
// Method:	CColorButton::GetDefaultColor()
// Notes:	None.
//***********************************************************************
COLORREF CColorButton::GetDefaultColor(void) const
{
	return m_DefaultColor;
}

//***********************************************************************
// Method:	CColorButton::SetDefaultColor()
// Notes:	None.
//***********************************************************************
void CColorButton::SetDefaultColor(COLORREF Color)
{
	m_DefaultColor = Color;
}

//***********************************************************************
// Method:	CColorButton::SetCustomText()
// Notes:	None.
//***********************************************************************
void CColorButton::SetCustomText(LPCTSTR tszText)
{
	m_strCustomText = tszText;
}

//***********************************************************************
// Method:	CColorButton::SetDefaultText()
// Notes:	None.
//***********************************************************************
void CColorButton::SetDefaultText(LPCTSTR tszText)
{
	m_strDefaultText = tszText;
}


//***********************************************************************
// Method:	CColorButton::SetTrackSelection()
// Notes:	None.
//***********************************************************************
void CColorButton::SetTrackSelection(BOOL bTrack)
{
	m_bTrackSelection = bTrack;
}

//***********************************************************************
// Method:	CColorButton::GetTrackSelection()
// Notes:	None.
//***********************************************************************
BOOL CColorButton::GetTrackSelection(void) const
{
	return m_bTrackSelection;
}

//***********************************************************************
//**                         CButton Overrides                         **
//***********************************************************************
void CColorButton::PreSubclassWindow() 
{
    ModifyStyle(0, BS_OWNERDRAW);      

    _Inherited::PreSubclassWindow();
}

//***********************************************************************
//**                         Message Handlers                         **
//***********************************************************************
BEGIN_MESSAGE_MAP(CColorButton, CButton)
    //{{AFX_MSG_MAP(CColorButton)
	ON_CONTROL_REFLECT_EX(BN_DBLCLK, OnDbClicked)
    ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
    //}}AFX_MSG_MAP
    ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()


//***********************************************************************
// Method:	CColorButton::OnSelEndOK()
// Notes:	None.
//***********************************************************************
LONG CColorButton::OnSelEndOK(UINT lParam, LONG /*wParam*/)
{
	m_bPopupActive = FALSE;

    COLORREF OldColor = m_Color;
	
	Color = (COLORREF)lParam;

    CWnd *pParent = GetParent()->GetParent();

    if (pParent) 
	{
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDOK, lParam, (WPARAM) GetDlgCtrlID());
    }

    if (OldColor != m_Color)
        if (pParent) pParent->SendMessage(CPN_SELCHANGE, m_Color, (WPARAM) GetDlgCtrlID());

    return TRUE;
}


//***********************************************************************
// Method:	CColorButton::OnSelEndCancel()
// Notes:	None.
//***********************************************************************
LONG CColorButton::OnSelEndCancel(UINT lParam, LONG /*wParam*/)
{
	m_bPopupActive = FALSE;
	
	Color = (COLORREF)lParam;

    CWnd *pParent = GetParent()->GetParent();

    if (pParent) 
	{
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDCANCEL, lParam, (WPARAM) GetDlgCtrlID());
    }

    return TRUE;
}


//***********************************************************************
// Method:	CColorButton::OnSelChange()
// Notes:	None.
//***********************************************************************
LONG CColorButton::OnSelChange(UINT lParam, LONG /*wParam*/)
{
    if (m_bTrackSelection) 
		Color = (COLORREF)lParam;

    CWnd *pParent = GetParent()->GetParent();

    if (pParent) pParent->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

//***********************************************************************
// Method:	CColorButton::OnCreate()
// Notes:	None.
//***********************************************************************
int CColorButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

//***********************************************************************
// Method:	CColorButton::OnDbClicked()
// Notes:	None.
//***********************************************************************
BOOL CColorButton::OnDbClicked()
{
// 	m_bPopupActive = TRUE;
// 
//     CRect rDraw;
//     GetWindowRect(rDraw);
// 
//     new CColourPopup(CPoint(rDraw.left, rDraw.bottom),		// Point to display popup
//                      m_Color,m_DefaultColor,								// Selected colour
//                      this,									// parent
//                      m_strDefaultText,						// "Default" text area
//                      m_strCustomText);						// Custom Text
// 
//     CWnd *pParent = GetParent()/*->GetParent()*/;

//     if (pParent)
//         pParent->SendMessage(CPN_DROPDOWN, (LPARAM)m_Color, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

void CColorButton::SelectColor()
{
	m_bSelected = true;
	CColorDialog colDlg;
	if (IDOK == colDlg.DoModal())
	{
		m_Color = colDlg.GetColor();

		ShowWindow(SW_HIDE);
		ShowWindow(SW_SHOW);

		SetFocus();
	}
}


//***********************************************************************
// Method:	CColorButton::DrawItem()
// Notes:	None.
//***********************************************************************
void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct);

	CDC*    pDC      = CDC::FromHandle(lpDrawItemStruct->hDC);
	UINT    state    = lpDrawItemStruct->itemState;
    CRect   rDraw    = lpDrawItemStruct->rcItem;
	CRect	rArrow;

	if (m_bPopupActive)
		state |= ODS_SELECTED|ODS_FOCUS;

	//state
	m_bIsPressed = (lpDrawItemStruct->itemState & ODS_SELECTED);

	m_bIsFocused  = (lpDrawItemStruct->itemState & ODS_FOCUS);
	m_bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);
	//******************************************************
	//**                  Draw Outer Edge
	//******************************************************
	UINT uFrameState = DFCS_BUTTONPUSH|DFCS_ADJUSTRECT;

	if (state & ODS_SELECTED)
		uFrameState |= DFCS_PUSHED;

	if (state & ODS_DISABLED)
		uFrameState |= DFCS_INACTIVE;
// 	
// 	pDC->DrawFrameControl(&rDraw,
// 						  DFC_BUTTON,
// 						  uFrameState);
// 
	OnDrawBorder(pDC,&rDraw);
/*	if (m_csBitmaps[0].hBitmap)
	{
		pDC->SetBkColor(RGB(255,255,255));
 		CRect rc(rDraw);
		CRect rbitmap = CRect(rDraw.left,
			rDraw.top,
			rDraw.right,
			rDraw.bottom/2);;
// 		rc.DeflateRect(5,5);
		DrawTheBitmap(pDC, TRUE, &lpDrawItemStruct->rcItem, &rbitmap, m_bIsPressed, m_bIsDisabled);
		//DrawTheBitmap(pDC, !sTitle.IsEmpty(), &lpDIS->rcItem, &captionRect, m_bIsPressed, m_bIsDisabled);
	} // if
*/



	
	if (state & ODS_SELECTED)
		rDraw.OffsetRect(1,1);

	//******************************************************
	//**                     Draw Focus
	//******************************************************
	if (m_bSelected) 
    {
		RECT rFocus = {rDraw.left-1,
					   rDraw.top-1,
					   rDraw.right+1 ,
					   rDraw.bottom+1};
  
		pDC->FillSolidRect(rFocus.left, rFocus.top, rFocus.right, rFocus.bottom, RGB(0,0,0));
    //    pDC->DrawFocusRect(&rFocus);
    }

	//rDraw.DeflateRect(::GetSystemMetrics(SM_CXEDGE),
	//				  ::GetSystemMetrics(SM_CYEDGE));

	//******************************************************
	//**                     Draw Arrow
	//******************************************************
	rArrow.left		= rDraw.right - g_ciArrowSizeX - ::GetSystemMetrics(SM_CXEDGE) /2;
	rArrow.right	= rArrow.left + g_ciArrowSizeX;
// 	rArrow.top		= (rDraw.bottom + rDraw.top)/2 - g_ciArrowSizeY / 2;
// 	rArrow.bottom	= (rDraw.bottom + rDraw.top)/2 + g_ciArrowSizeY / 2;

// 	DrawArrow(pDC,
// 			  &rArrow,
// 			  0,
// 			  (state & ODS_DISABLED) 
// 			  ? ::GetSysColor(COLOR_GRAYTEXT) 
// 			  : RGB(0,0,0));


//	rDraw.right = rArrow.left - ::GetSystemMetrics(SM_CXEDGE)/2;

	//******************************************************
	//**                   Draw Separator
	//******************************************************
// 	pDC->DrawEdge(&rDraw,
// 				  EDGE_ETCHED,
// 				  BF_RIGHT);
// 
// 	rDraw.right -= (::GetSystemMetrics(SM_CXEDGE) * 2) + 1 ;
				  
	//******************************************************
	//**                     Draw Color
	//******************************************************
// 	if ((state & ODS_DISABLED) == 0)
// 	{
		rDraw.DeflateRect(::GetSystemMetrics(SM_CXEDGE)*2,
								  ::GetSystemMetrics(SM_CYEDGE)*2);

 		CRect rbitmap = CRect(rDraw.left-1,
			rDraw.top-1,
 			rDraw.right+1,
			rDraw.bottom+1);;
		pDC->FillSolidRect(&rbitmap,
						   (m_Color == CLR_DEFAULT)
						   ? m_DefaultColor
						   : m_Color);

// 		::FrameRect(pDC->m_hDC,
// 					&rDraw,
// 					(HBRUSH)::GetStockObject(BLACK_BRUSH));
// 	}
}


//***********************************************************************
//**                          Static Methods                          **
//***********************************************************************

//***********************************************************************
// Method:	CColorButton::DrawArrow()
// Notes:	None.
//***********************************************************************
void CColorButton::DrawArrow(CDC* pDC, 
							 RECT* pRect, 
							 int iDirection,
							 COLORREF clrArrow /*= RGB(0,0,0)*/)
{
	POINT ptsArrow[3];

	switch (iDirection)
	{
		case 0 : // Down
		{
			ptsArrow[0].x = pRect->left;
			ptsArrow[0].y = pRect->top;
			ptsArrow[1].x = pRect->right;
			ptsArrow[1].y = pRect->top;
			ptsArrow[2].x = (pRect->left + pRect->right)/2;
			ptsArrow[2].y = pRect->bottom;
			break;
		}

		case 1 : // Up
		{
			ptsArrow[0].x = pRect->left;
			ptsArrow[0].y = pRect->bottom;
			ptsArrow[1].x = pRect->right;
			ptsArrow[1].y = pRect->bottom;
			ptsArrow[2].x = (pRect->left + pRect->right)/2;
			ptsArrow[2].y = pRect->top;
			break;
		}

		case 2 : // Left
		{
			ptsArrow[0].x = pRect->right;
			ptsArrow[0].y = pRect->top;
			ptsArrow[1].x = pRect->right;
			ptsArrow[1].y = pRect->bottom;
			ptsArrow[2].x = pRect->left;
			ptsArrow[2].y = (pRect->top + pRect->bottom)/2;
			break;
		}

		case 3 : // Right
		{
			ptsArrow[0].x = pRect->left;
			ptsArrow[0].y = pRect->top;
			ptsArrow[1].x = pRect->left;
			ptsArrow[1].y = pRect->bottom;
			ptsArrow[2].x = pRect->right;
			ptsArrow[2].y = (pRect->top + pRect->bottom)/2;
			break;
		}
	}
	
	CBrush brsArrow(clrArrow);
	CPen penArrow(PS_SOLID, 1 , clrArrow);

	CBrush* pOldBrush = pDC->SelectObject(&brsArrow);
	CPen*   pOldPen   = pDC->SelectObject(&penArrow);
	
	pDC->SetPolyFillMode(WINDING);
	pDC->Polygon(ptsArrow, 3);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}
DWORD CColorButton::OnDrawBorder(CDC* pDC, CRect* pRect)
{
	// Draw pressed button
	if (m_bIsPressed)
	{
		if (m_bIsFlat)
		{
			if (m_bDrawBorder)
				pDC->Draw3dRect(pRect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT));
		}
		else    
		{
			CBrush brBtnShadow(GetSysColor(COLOR_BTNSHADOW));
			pDC->FrameRect(pRect, &brBtnShadow);
		}
	}
	else // ...else draw non pressed button
	{
		CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White
		CPen pen3DLight(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));       // Light gray
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		CPen pen3DDKShadow(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black

		if (m_bIsFlat)
		{
			if (m_bMouseOnButton && m_bDrawBorder)
				pDC->Draw3dRect(pRect, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));
		}
		else
		{
			// Draw top-left borders
			// White line
			CPen* pOldPen = pDC->SelectObject(&penBtnHiLight);
			pDC->MoveTo(pRect->left, pRect->bottom-1);
			pDC->LineTo(pRect->left, pRect->top);
			pDC->LineTo(pRect->right, pRect->top);
			// Light gray line
			pDC->SelectObject(pen3DLight);
			pDC->MoveTo(pRect->left+1, pRect->bottom-1);
			pDC->LineTo(pRect->left+1, pRect->top+1);
			pDC->LineTo(pRect->right, pRect->top+1);
			// Draw bottom-right borders
			// Black line
			pDC->SelectObject(pen3DDKShadow);
			pDC->MoveTo(pRect->left, pRect->bottom-1);
			pDC->LineTo(pRect->right-1, pRect->bottom-1);
			pDC->LineTo(pRect->right-1, pRect->top-1);
			// Dark gray line
			pDC->SelectObject(penBtnShadow);
			pDC->MoveTo(pRect->left+1, pRect->bottom-2);
			pDC->LineTo(pRect->right-2, pRect->bottom-2);
			pDC->LineTo(pRect->right-2, pRect->top);
			//
			pDC->SelectObject(pOldPen);
		} // else
	} // else

	return 0;
} // End of OnDrawBorder

void CColorButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd*				wndUnderMouse = NULL;
	CWnd*				wndActive = this;
	TRACKMOUSEEVENT		csTME;

	CButton::OnMouseMove(nFlags, point);

	ClientToScreen(&point);
	wndUnderMouse = WindowFromPoint(point);

	// If the mouse enter the button with the left button pressed then do nothing
	if (nFlags & MK_LBUTTON && m_bMouseOnButton == FALSE) return;

	// If our button is not flat then do nothing
	if (m_bIsFlat == FALSE) return;

	if (m_bAlwaysTrack == FALSE)	wndActive = GetActiveWindow();

	if (wndUnderMouse && wndUnderMouse->m_hWnd == m_hWnd && wndActive)
	{
		if (!m_bMouseOnButton)
		{
			m_bMouseOnButton = TRUE;

			Invalidate();

// #ifdef	BTNST_USE_SOUND
// 			// Play sound ?
// 			if (m_csSounds[0].lpszSound)
// 				::PlaySound(m_csSounds[0].lpszSound, m_csSounds[0].hMod, m_csSounds[0].dwFlags);
// #endif

			csTME.cbSize = sizeof(csTME);
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = m_hWnd;
			::_TrackMouseEvent(&csTME);
		} // if
	} 
	else 
	{
		if (m_bIsFlat)
		{
			if (m_bMouseOnButton)
			{
				m_bMouseOnButton = FALSE;
				Invalidate();
			} // if
		} // if

	}
} 
LRESULT CColorButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if (m_bIsFlat)
	{
		if (m_bMouseOnButton)
		{
			m_bMouseOnButton = FALSE;
			Invalidate();
		} // if
	} // if

	return 0;
} //
void CColorButton::DrawTheBitmap(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled)
{
	HDC			hdcBmpMem	= NULL;
	HBITMAP		hbmOldBmp	= NULL;
	HDC			hdcMem		= NULL;
	HBITMAP		hbmT		= NULL;

	BYTE		byIndex		= 0;

	// Select the bitmap to use
	// 	if ((m_bIsCheckBox && bIsPressed) || (!m_bIsCheckBox && (bIsPressed || m_bMouseOnButton)))
	// 		byIndex = 0;
	// 	else
	// 		byIndex = (m_csBitmaps[1].hBitmap == NULL ? 0 : 1);
	if (bIsDisabled)
		byIndex=1;
	CRect	rImage;
	PrepareImageRect(bHasTitle, rpItem, rpCaption, bIsPressed, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, &rImage);

	hdcBmpMem = ::CreateCompatibleDC(pDC->m_hDC);

	hbmOldBmp = (HBITMAP)::SelectObject(hdcBmpMem, m_csBitmaps[byIndex].hBitmap);

	hdcMem = ::CreateCompatibleDC(NULL);

	hbmT = (HBITMAP)::SelectObject(hdcMem, m_csBitmaps[byIndex].hMask);

	if (bIsDisabled && m_bShowDisabledBitmap)
	{

		::BitBlt(pDC->m_hDC, rImage.left, rImage.top, m_csBitmaps[1].dwWidth, m_csBitmaps[1].dwHeight, hdcMem, 0, 0, SRCAND);

		::BitBlt(pDC->m_hDC, rImage.left, rImage.top, m_csBitmaps[1].dwWidth, m_csBitmaps[1].dwHeight, hdcBmpMem, 0, 0, SRCPAINT);
	} // if
	//else
	{
		
		::BitBlt(pDC->m_hDC, rImage.left, rImage.top, m_csBitmaps[0].dwWidth, m_csBitmaps[0].dwHeight, hdcMem, 0, 0, SRCAND);

		::BitBlt(pDC->m_hDC, rImage.left, rImage.top, m_csBitmaps[0].dwWidth, m_csBitmaps[0].dwHeight, hdcBmpMem, 0, 0, SRCPAINT);
	} // else

	::SelectObject(hdcMem, hbmT);
	::DeleteDC(hdcMem);

	::SelectObject(hdcBmpMem, hbmOldBmp);
	::DeleteDC(hdcBmpMem);
} // End of DrawTheBitmap

void CColorButton::PrepareImageRect(BOOL bHasTitle, RECT* rpItem, CRect* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, CRect* rpImage)
{
	CRect rBtn;

	rpImage->CopyRect(rpItem);

	switch (m_byAlign)
	{
	case ST_ALIGN_HORIZ:
		if (bHasTitle == FALSE)
		{
			// Center image horizontally
			rpImage->left += ((rpImage->Width() - (long)dwWidth)/2);
		}
		else
		{
			// Image must be placed just inside the focus rect
			rpImage->left += m_ptImageOrg.x;  
			rpTitle->left += dwWidth + m_ptImageOrg.x;
		}
		// Center image vertically
		rpImage->top += ((rpImage->Height() - (long)dwHeight)/2);
		break;

	case ST_ALIGN_HORIZ_RIGHT:
		GetClientRect(&rBtn);
		if (bHasTitle == FALSE)
		{
			// Center image horizontally
			rpImage->left += ((rpImage->Width() - (long)dwWidth)/2);
		}
		else
		{
			// Image must be placed just inside the focus rect
			rpTitle->right = rpTitle->Width() - dwWidth - m_ptImageOrg.x;
			rpTitle->left = m_ptImageOrg.x;
			rpImage->left = rBtn.right - dwWidth - m_ptImageOrg.x;
			// Center image vertically
			rpImage->top += ((rpImage->Height() - (long)dwHeight)/2);
		}
		break;

	case ST_ALIGN_VERT:
		// Center image horizontally
		rpImage->left += ((rpImage->Width() - (long)dwWidth)/2);
		if (bHasTitle == FALSE)
		{
			// Center image vertically
			rpImage->top += ((rpImage->Height() - (long)dwHeight)/2);           
		}
		else
		{
			rpImage->top = m_ptImageOrg.y;
			rpTitle->top += dwHeight;
		}
		break;

	case ST_ALIGN_OVERLAP:
		break;
	} // switch

	// If button is pressed then press image also
// 	if (bIsPressed && m_bIsCheckBox == FALSE)
// 		rpImage->OffsetRect(m_ptPressedOffset.x, m_ptPressedOffset.y);
} // End of PrepareImageRect
HBITMAP CColorButton::CreateBitmapMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTransColor)
{
	HBITMAP		hMask		= NULL;
	HDC			hdcSrc		= NULL;
	HDC			hdcDest		= NULL;
	HBITMAP		hbmSrcT		= NULL;
	HBITMAP		hbmDestT	= NULL;
	COLORREF	crSaveBk;
	COLORREF	crSaveDestText;

	hMask = ::CreateBitmap(dwWidth, dwHeight, 1, 1, NULL);
	if (hMask == NULL)	return NULL;

	hdcSrc	= ::CreateCompatibleDC(NULL);
	hdcDest	= ::CreateCompatibleDC(NULL);

	hbmSrcT = (HBITMAP)::SelectObject(hdcSrc, hSourceBitmap);
	hbmDestT = (HBITMAP)::SelectObject(hdcDest, hMask);

	crSaveBk = ::SetBkColor(hdcSrc, crTransColor);

	::BitBlt(hdcDest, 0, 0, dwWidth, dwHeight, hdcSrc, 0, 0, SRCCOPY);

	crSaveDestText = ::SetTextColor(hdcSrc, RGB(255, 255, 255));
	::SetBkColor(hdcSrc,RGB(0, 0, 0));

	::BitBlt(hdcSrc, 0, 0, dwWidth, dwHeight, hdcDest, 0, 0, SRCAND);

	SetTextColor(hdcDest, crSaveDestText);

	::SetBkColor(hdcSrc, crSaveBk);
	::SelectObject(hdcSrc, hbmSrcT);
	::SelectObject(hdcDest, hbmDestT);

	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDest);

	return hMask;
} // End of CreateBitmapMask
DWORD CColorButton::SetBitmaps(int nBitmapIn, COLORREF crTransColorIn, int nBitmapOut, COLORREF crTransColorOut)
{
	HBITMAP		hBitmapIn		= NULL;
	HBITMAP		hBitmapOut		= NULL;
	HINSTANCE	hInstResource	= NULL;

	// Find correct resource handle
	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nBitmapIn), RT_BITMAP);

	// Load bitmap In
	hBitmapIn = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapIn), IMAGE_BITMAP, 0, 0, 0);

	// Load bitmap Out
	switch (nBitmapOut)
	{
	case NULL:
		break;
	case (int)BTNST_AUTO_GRAY:
		hBitmapOut = (HBITMAP)BTNST_AUTO_GRAY;
		break;
	case (int)BTNST_AUTO_DARKER:
		hBitmapOut = (HBITMAP)BTNST_AUTO_DARKER;
		break;
	default:
		hBitmapOut = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapOut), IMAGE_BITMAP, 0, 0, 0);
		break;
	} // if

	return SetBitmaps(hBitmapIn, crTransColorIn, hBitmapOut, crTransColorOut);
} // End of SetBitmaps
DWORD CColorButton::SetBitmaps(HBITMAP hBitmapIn, COLORREF crTransColorIn, HBITMAP hBitmapOut, COLORREF crTransColorOut)
{
	int		nRetValue = 0;
	nRetValue;
	BITMAP	csBitmapSize;

	// Free any loaded resource
	FreeResources();

	if (hBitmapIn)
	{
		m_csBitmaps[0].hBitmap = hBitmapIn;
		m_csBitmaps[0].crTransparent = crTransColorIn;
		// Get bitmap size
		nRetValue = ::GetObject(hBitmapIn, sizeof(csBitmapSize), &csBitmapSize);
		if (nRetValue == 0)
		{
			FreeResources();
			return BTNST_INVALIDRESOURCE;
		} // if
		m_csBitmaps[0].dwWidth = (DWORD)csBitmapSize.bmWidth;
		m_csBitmaps[0].dwHeight = (DWORD)csBitmapSize.bmHeight;

		// Create grayscale/darker bitmap BEFORE mask (of hBitmapIn)
		crTransColorOut = crTransColorIn;
		// Create mask for bitmap In
		m_csBitmaps[0].hMask = CreateBitmapMask(hBitmapIn, m_csBitmaps[0].dwWidth, m_csBitmaps[0].dwHeight, crTransColorIn);
		if (m_csBitmaps[0].hMask == NULL)
		{
			FreeResources();
			return BTNST_FAILEDMASK;
		} // if

		if (hBitmapOut)
		{
			m_csBitmaps[1].hBitmap = hBitmapOut;
			m_csBitmaps[1].crTransparent = crTransColorOut;
			// Get bitmap size
			nRetValue = ::GetObject(hBitmapOut, sizeof(csBitmapSize), &csBitmapSize);
			if (nRetValue == 0)
			{
				FreeResources();
				return BTNST_INVALIDRESOURCE;
			} // if
			m_csBitmaps[1].dwWidth = (DWORD)csBitmapSize.bmWidth;
			m_csBitmaps[1].dwHeight = (DWORD)csBitmapSize.bmHeight;

			// Create mask for bitmap Out
			m_csBitmaps[1].hMask = CreateBitmapMask(hBitmapOut, m_csBitmaps[1].dwWidth, m_csBitmaps[1].dwHeight, crTransColorOut);
			if (m_csBitmaps[1].hMask == NULL)
			{
				FreeResources();
				return BTNST_FAILEDMASK;
			} // if
		} // if
	} // if

	Invalidate();

	return BTNST_OK;
} // End of SetBitmaps
void CColorButton::FreeResources(BOOL bCheckForNULL)
{
	if (bCheckForNULL)
	{
		// Destroy icons
		// Note: the following two lines MUST be here! even if
		// BoundChecker says they are unnecessary!
// 		if (m_csIcons[0].hIcon)	::DestroyIcon(m_csIcons[0].hIcon);
// 		if (m_csIcons[1].hIcon)	::DestroyIcon(m_csIcons[1].hIcon);

		// Destroy bitmaps
		if (m_csBitmaps[0].hBitmap)	::DeleteObject(m_csBitmaps[0].hBitmap);
		if (m_csBitmaps[1].hBitmap)	::DeleteObject(m_csBitmaps[1].hBitmap);

		// Destroy mask bitmaps
		if (m_csBitmaps[0].hMask)	::DeleteObject(m_csBitmaps[0].hMask);
		if (m_csBitmaps[1].hMask)	::DeleteObject(m_csBitmaps[1].hMask);
	} // if

//	::ZeroMemory(&m_csIcons, sizeof(m_csIcons));
	::ZeroMemory(&m_csBitmaps, sizeof(m_csBitmaps));
} // End of FreeResources

DWORD CColorButton::SetPressedStyle(BYTE byStyle, BOOL bRepaint)
{
	switch (byStyle)
	{
	case BTNST_PRESSED_LEFTRIGHT:
		m_ptPressedOffset.x = 1;
		m_ptPressedOffset.y = 1;
		break;
	case BTNST_PRESSED_TOPBOTTOM:
		m_ptPressedOffset.x = 0;
		m_ptPressedOffset.y = 2;
		break;
	default:
		return BTNST_INVALIDPRESSEDSTYLE;
	} // switch

	if (bRepaint)	Invalidate();

	return BTNST_OK;
} // End of SetPressedStyle

void CColorButton::SetSelected( bool bSel )
{
	m_bSelected = bSel;
}