//#include "stdafx.h"  //old vs
#include "pch.h"	   //new vs

#include "winuser.h"
#include "CXListCtrl.h"
#include "CXThemeHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl
IMPLEMENT_DYNCREATE(CXListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(CXListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CXListCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_NCMOUSEMOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SYSCOLORCHANGE()
	ON_MESSAGE(LVM_SETCOLUMN, OnSetColumn)
	ON_MESSAGE(LVM_INSERTCOLUMN, OnInsertColumn)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, OnInsertItem)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteItem)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
CXListCtrl::CXListCtrl()
{
	m_nEditItem    = -1;
	m_nEditSubItem = -1;
	m_bProcessEditCtrlChange = FALSE;
	m_bProcessComboBoxChange = FALSE;
	m_bProcessHideEditCtrl = FALSE;
	m_bProcessHideComboBox = FALSE;
	m_pEditWnd = NULL;
	m_pComboBoxWnd = NULL;

	m_cr3DFace              = ::GetSysColor(COLOR_3DFACE);
	m_cr3DHighLight         = ::GetSysColor(COLOR_3DHIGHLIGHT);
	m_cr3DShadow            = ::GetSysColor(COLOR_3DSHADOW);
	m_crBtnFace             = ::GetSysColor(COLOR_BTNFACE);
	m_crBtnShadow           = ::GetSysColor(COLOR_BTNSHADOW);
	m_crBtnText             = ::GetSysColor(COLOR_BTNTEXT);
	m_crGrayText            = ::GetSysColor(COLOR_GRAYTEXT);
	m_crHighLight           = ::GetSysColor(COLOR_HIGHLIGHT);
	m_crHighLightText       = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_crWindow              = ::GetSysColor(COLOR_WINDOW);
	m_crWindowText          = ::GetSysColor(COLOR_WINDOWTEXT);
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXListCtrl::~CXListCtrl()
{
}

BOOL CXListCtrl::CheckItemIndex(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	return ((nItem >= 0) && (nItem < GetItemCount()));
}

BOOL CXListCtrl::CheckSubItemIndex(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
	{
		return FALSE;
	}

	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumnCount());
	if ((nSubItem < 0) || (nSubItem >= GetColumnCount()))
	{
		return FALSE;
	}

	return TRUE;
}

XLISTCTRL_ITEM_DATA* CXListCtrl::GetXListCtrlData(int nItem)
{
	if ((nItem >= 0) && (nItem < m_arItemData.GetCount()))
	{
		return m_arItemData.GetAt(nItem);
	}

	ASSERT(FALSE);
	return NULL;
}

BOOL CXListCtrl::GetEditCtrlInfo(int iItem, int iSubItem, CString& strText)
{
	strText.Empty();

	if (CheckSubItemIndex(iItem, iSubItem))
	{
		strText = GetItemText(iItem, iSubItem);
		return TRUE;
	}

	return FALSE;
}

BOOL CXListCtrl::ShowEditCtrl(int iItem, int iSubItem)
{
	HideAllCtrls();

	try
	{
		m_pEditWnd = new CEdit();
	}
	catch (...)
	{
		m_pEditWnd = NULL;
	}

	if (m_pEditWnd == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	DWORD dwStyle = 0;
	DWORD dwStyleEx = 0;
	CRect rcSubItem(0,0,0,0);

	dwStyle = WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	dwStyleEx = WS_EX_CLIENTEDGE;

	GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, rcSubItem);
	rcSubItem.DeflateRect(0,0,1,1);

	if (!m_pEditWnd->CreateEx(dwStyleEx, _T("EDIT"), NULL, dwStyle, rcSubItem, this, 1))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CString strText = _T("");
	GetEditCtrlInfo(iItem, iSubItem, strText);

	m_nEditItem = iItem;
	m_nEditSubItem = iSubItem;
	m_pEditWnd->SetFont(GetFont());
	m_pEditWnd->ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_pEditWnd->SetWindowText(strText);
	m_pEditWnd->ShowWindow(SW_SHOW);
	m_pEditWnd->SetSel(0, -1);
	m_pEditWnd->SetFocus();
	return TRUE;
}

BOOL CXListCtrl::OnEditCtrlChange()
{
	BOOL bReturn = FALSE;
	ASSERT(m_pEditWnd != NULL);
	if ((!m_bProcessEditCtrlChange) && (m_pEditWnd != NULL))
	{
		m_bProcessEditCtrlChange = TRUE;

		ASSERT(::IsWindow(m_pEditWnd->GetSafeHwnd()));
		if (CheckSubItemIndex(m_nEditItem, m_nEditSubItem))
		{
			CString strText = _T("");
			m_pEditWnd->GetWindowText(strText);
			if (CheckEditInfo(m_nEditItem, m_nEditSubItem, strText))
			{
				UpdateEditInfo(m_nEditItem, m_nEditSubItem, strText);
				bReturn = TRUE;
			}
		}

		m_bProcessEditCtrlChange = FALSE;
	}

	return bReturn;
}

void CXListCtrl::HideEditCtrl()
{
	if ((!m_bProcessHideEditCtrl) && (m_pEditWnd != NULL))
	{
		m_bProcessHideEditCtrl = TRUE;

		HWND hWndEditCtrl = NULL;
		CRect rcEditCtrl(0,0,0,0);

		hWndEditCtrl = m_pEditWnd->GetSafeHwnd();
		ASSERT(::IsWindow(hWndEditCtrl));
		if (::IsWindow(hWndEditCtrl))
		{
			if (::IsWindowVisible(hWndEditCtrl))
			{
				m_pEditWnd->GetWindowRect(&rcEditCtrl);
				m_pEditWnd->ShowWindow(SW_HIDE);

				ScreenToClient(&rcEditCtrl);
				InvalidateRect(&rcEditCtrl);
			}

			UpdateItem(m_nEditItem, m_nEditSubItem);
			m_pEditWnd->DestroyWindow();

			if (IsWindow(this->GetParent()->m_hWnd))
				::PostMessage(this->GetParent()->m_hWnd, DF_LIST_MESSAGE_EDIT_HIDE, (WPARAM)m_nEditItem, (LPARAM)m_nEditSubItem);  //Send Message to Parent
		}

		m_nEditItem = -1;
		m_nEditSubItem =  -1;
		delete m_pEditWnd;
		m_pEditWnd = NULL;

		m_bProcessHideEditCtrl = FALSE;
	}
}

BOOL CXListCtrl::GetComboBoxInfo(int iItem, int iSubItem, CStringArray& arText, int& nSelectIndex)
{
	nSelectIndex = 0;
	arText.RemoveAll();

	if (CheckSubItemIndex(iItem, iSubItem))
	{
		GetComboBoxArrayInfo(iItem,iSubItem,arText);

		CString strString = GetItemText(iItem, iSubItem);
		nSelectIndex = GetIndexFromArray(arText,strString);

		return TRUE;
	}

	return FALSE;
}

BOOL CXListCtrl::SetComboBoxArrayInfo(CStringArray& arText)
{
	for (int i = 0; i < DF_MAX_ROW; i++)
	{
		for (int j = 0; j < DF_MAX_COL; j++)
		{
			m_comboBoxArray[i][j].Copy(arText);
		}
	}

	return TRUE;
}

BOOL CXListCtrl::SetComboBoxArrayInfo(int iItem, int iSubItem, CStringArray& arText)
{
	if (iItem >= DF_MAX_ROW || iSubItem >= DF_MAX_COL)
		return FALSE;

	m_comboBoxArray[iItem][iSubItem].Copy(arText);

	return TRUE;
}

BOOL CXListCtrl::GetComboBoxArrayInfo(int iItem, int iSubItem, CStringArray& arText)
{
	if (iItem >= DF_MAX_ROW || iSubItem >= DF_MAX_COL)
		return FALSE;

 	arText.Copy(m_comboBoxArray[iItem][iSubItem]);

	return TRUE;
}

int CXListCtrl::GetIndexFromArray(CStringArray& arText, CString strString)
{
	if (!strString.IsEmpty())
	{
		for (int nIndex = 0; nIndex < arText.GetSize(); nIndex++)
		{
			if (strString.CompareNoCase(arText.GetAt(nIndex)) == 0)
			{
				return nIndex;
			}
		}
	}
	
	return 0;
}

BOOL CXListCtrl::ShowComboBox(int iItem, int iSubItem)
{
	HideAllCtrls();

	int nCount = 0;
	int nSelectIndex = 0;
	CStringArray arText;

	GetComboBoxInfo(iItem, iSubItem, arText, nSelectIndex);
	nCount = arText.GetCount();
	if (nCount <= 0)
	{
		return FALSE;
	}

	DWORD dwStyle = 0;
	int nCXVScroll = 0;
	CRect rcSubItem(0,0,0,0);

	nCXVScroll = ::GetSystemMetrics(SM_CXVSCROLL);
	dwStyle = WS_CHILD|CBS_DROPDOWNLIST|WS_VSCROLL|CBS_NOINTEGRALHEIGHT;

	GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, rcSubItem);
	rcSubItem.DeflateRect(0,0,1,1);
	rcSubItem.InflateRect(1,1,0,0);

	delete m_pComboBoxWnd;
	m_pComboBoxWnd = new CComboBox();

	COMBOBOXINFO cbInfo;
	cbInfo.cbSize = sizeof(COMBOBOXINFO);

	if (m_pComboBoxWnd->Create(dwStyle, rcSubItem, this, 1))
	{
		m_pComboBoxWnd->SetFont(GetFont());
		if (m_pComboBoxWnd->GetComboBoxInfo(&cbInfo))
		{
			LOGFONT logFont;
			CFont newFont;
			CFont* pCurFont = GetFont();
			pCurFont->GetLogFont(&logFont);
			logFont.lfHeight = 12;
			newFont.CreateFontIndirect(&logFont);
			::SendMessage(cbInfo.hwndList, WM_SETFONT, (WPARAM)(newFont.GetSafeHandle()), FALSE);
		}

		rcSubItem.bottom = rcSubItem.bottom + 100;
		m_pComboBoxWnd->MoveWindow(rcSubItem, FALSE);
	}
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CDC dcComboBoxList;
	HDC hWndListDC = NULL;

	cbInfo.cbSize = sizeof(COMBOBOXINFO);
	if (m_pComboBoxWnd->GetComboBoxInfo(&cbInfo))
	{
		hWndListDC = ::GetDC(cbInfo.hwndList);
		dcComboBoxList.Attach(hWndListDC);
	}

	int nIndex = 0;
	int nStrWidth = 0;
	int nMaxStrWidth = 0;
	CString strString = _T("");

	for (nIndex = 0; nIndex < nCount; ++nIndex)
	{
		strString = arText.GetAt(nIndex);
		m_pComboBoxWnd->AddString(strString);
		m_pComboBoxWnd->SetItemHeight(nIndex, 16);

		if (dcComboBoxList.GetSafeHdc() != NULL)
		{
			nStrWidth = dcComboBoxList.GetTextExtent(strString).cx;
		}
		else
		{
			nStrWidth = GetStringWidth(strString);
		}

		if (nStrWidth > nMaxStrWidth)
		{
			nMaxStrWidth = nStrWidth;
		}
	}

	if (dcComboBoxList.GetSafeHdc() != NULL)
	{
		dcComboBoxList.Detach();
		::ReleaseDC(cbInfo.hwndList, hWndListDC);
	}

	m_pComboBoxWnd->MoveWindow(rcSubItem, FALSE);

	if ((nMaxStrWidth + nCXVScroll + 10) > rcSubItem.Width())
	{
		nMaxStrWidth += (nCXVScroll + 10);
		m_pComboBoxWnd->SetDroppedWidth(nMaxStrWidth);
	}

	m_nEditItem = iItem;
	m_nEditSubItem = iSubItem;
	m_pComboBoxWnd->SetCurSel(nSelectIndex);
	m_pComboBoxWnd->ShowWindow(SW_SHOW);
//	m_pComboBoxWnd->ShowDropDown();
	m_pComboBoxWnd->SetFocus();
	return TRUE;
}

BOOL CXListCtrl::OnComboBoxChange()
{
	BOOL bReturn = FALSE;
	ASSERT(m_pComboBoxWnd != NULL);
	if ((!m_bProcessComboBoxChange) && (m_pComboBoxWnd != NULL))
	{
		m_bProcessComboBoxChange = TRUE;

		ASSERT(::IsWindow(m_pComboBoxWnd->GetSafeHwnd()));
		if (CheckSubItemIndex(m_nEditItem, m_nEditSubItem))
		{
			int nSelIndex = 0;
			CString strSelText = _T("");

			nSelIndex = m_pComboBoxWnd->GetCurSel();
			m_pComboBoxWnd->GetLBText(nSelIndex, strSelText);

			if (CheckEditInfo(m_nEditItem, m_nEditSubItem, strSelText))
			{
				UpdateEditInfo(m_nEditItem, m_nEditSubItem, strSelText);
				bReturn = TRUE;
			}
		}

		m_bProcessComboBoxChange = FALSE;
	}

	return bReturn;
}

void CXListCtrl::HideComboBox()
{
	if ((!m_bProcessHideComboBox) && (m_pComboBoxWnd != NULL))
	{
		m_bProcessHideComboBox = TRUE;

		HWND hWndComboBox = NULL;
		CRect rcComboBox(0,0,0,0);

		hWndComboBox = m_pComboBoxWnd->GetSafeHwnd();
		ASSERT(::IsWindow(hWndComboBox));
		if (::IsWindow(hWndComboBox))
		{
			if (::IsWindowVisible(hWndComboBox))
			{
				m_pComboBoxWnd->GetWindowRect(&rcComboBox);
				m_pComboBoxWnd->ShowWindow(SW_HIDE);

				ScreenToClient(&rcComboBox);
				InvalidateRect(&rcComboBox);
			}

			UpdateItem(m_nEditItem, m_nEditSubItem);
			m_pComboBoxWnd->DestroyWindow();
		}

		m_nEditItem = -1;
		m_nEditSubItem =  -1;
		delete m_pComboBoxWnd;
		m_pComboBoxWnd = NULL;

		m_bProcessHideComboBox = FALSE;
	}
}

void CXListCtrl::HideAllCtrls()
{
	HideEditCtrl();
	HideComboBox();
}

BOOL CXListCtrl::CheckEditInfo(int iItem, int iSubItem, LPCTSTR lpszNewText)
{
	iItem;
	iSubItem;
	lpszNewText;
	return TRUE;
}

void CXListCtrl::UpdateEditInfo(int iItem, int iSubItem, LPCTSTR lpszNewText)
{
	if (CheckSubItemIndex(m_nEditItem, m_nEditSubItem))
	{
		SetItemText(iItem, iSubItem, lpszNewText);
		UpdateItem(iItem, iSubItem);
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL CXListCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			if (pMsg->wParam == VK_RETURN)
			{
				if ((m_pComboBoxWnd != NULL) && (pMsg->hwnd == m_pComboBoxWnd->m_hWnd))
				{
					if ((m_pComboBoxWnd->GetStyle() & ES_WANTRETURN) == 0)
					{
						HideComboBox();
						SetFocus();
						return TRUE;
					}
				}

				if ((m_pEditWnd != NULL) && (pMsg->hwnd == m_pEditWnd->m_hWnd))
				{
					if ((m_pEditWnd->GetStyle() & ES_WANTRETURN) == 0)
					{
						OnEditCtrlChange();
						HideEditCtrl();
						SetFocus();
						return TRUE;
					}
				}
			}
			else if (pMsg->wParam == VK_ESCAPE)
			{
				if ((m_pComboBoxWnd != NULL) && (pMsg->hwnd == m_pComboBoxWnd->m_hWnd))
				{
					HideComboBox();
					return TRUE;
				}

				if ((m_pEditWnd != NULL) && (pMsg->hwnd == m_pEditWnd->m_hWnd))
				{
					HideEditCtrl();
					return TRUE;
				}
			}

			break;
		}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
		{
			if (m_pEditWnd != NULL)
			{
				//Update Edit Text
				if (CheckSubItemIndex(m_nEditItem, m_nEditSubItem))
				{
					XLISTCTRL_ITEM_DATA* pXLCD = NULL;
					pXLCD = GetXListCtrlData(m_nEditItem);
					if (pXLCD != NULL)
					{
						UINT uiEditType = pXLCD->arSubItemData[m_nEditSubItem]->uiEditType;

						CString strText = _T(""), strFinalText = _T("");
						m_pEditWnd->GetWindowText(strText);
						if (uiEditType == EditType_Text)
						{
							strFinalText = strText;
						}
						if (uiEditType == EditType_Int)
						{
							int nValue = _ttoi(strText);
							strFinalText.Format(_T("%d"), nValue);
						}
						if (uiEditType == EditType_Double)
						{
							double dValue = _ttof(strText);
							strFinalText.Format(_T("%.1f"), dValue);
						}
						m_pEditWnd->SetWindowText(strFinalText);
					}
				}

				if (m_pEditWnd->GetSafeHwnd() != pMsg->hwnd)
				{
					if (!OnEditCtrlChange())
					{
						return TRUE;
					}

					HideEditCtrl();
					SetFocus();
				}
			}

			if (m_pComboBoxWnd != NULL)
			{
				if (m_pComboBoxWnd->GetSafeHwnd() != pMsg->hwnd)
				{
					HideComboBox();
					SetFocus();
				}
			}

			break;
		}
	case WM_KILLFOCUS:
		{
			if (m_pEditWnd != NULL)
			{
				if (m_pEditWnd->GetSafeHwnd() == pMsg->hwnd)
				{
					OnEditCtrlChange();
					HideEditCtrl();
					return TRUE;
				}
			}

			if (m_pComboBoxWnd != NULL)
			{
				if (m_pComboBoxWnd->GetSafeHwnd() == pMsg->hwnd)
				{
					HideComboBox();
					return TRUE;
				}
			}

			break;
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

BOOL CXListCtrl::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return CListCtrl::OnWndMsg(message, wParam, lParam, pResult);
}

void CXListCtrl::OnParentNotify(UINT message, LPARAM lParam)
{
	CListCtrl::OnParentNotify(message, lParam);
}

BOOL CXListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL bReturn = FALSE;
	bReturn = CListCtrl::OnCommand(wParam, lParam);

	HWND hCtrlWnd = NULL;
	HWND hFromWnd = NULL;
	UINT nNotificationCode = 0;

	hFromWnd = (HWND)(lParam);
	nNotificationCode = HIWORD(wParam);

	switch (nNotificationCode)
	{
	case EN_CHANGE:
		{
			ASSERT(m_pEditWnd != NULL);
			if (m_pEditWnd != NULL)
			{
				hCtrlWnd = m_pEditWnd->GetSafeHwnd();
				if ((hCtrlWnd != NULL) && (hCtrlWnd == hFromWnd))
				{
					OnEditCtrlChange();
				}
			}

			break;
		}
	case CBN_SELENDOK:
		{
			ASSERT(m_pComboBoxWnd != NULL);
			if (m_pComboBoxWnd != NULL)
			{
				hCtrlWnd = m_pComboBoxWnd->GetSafeHwnd();
				if ((hCtrlWnd != NULL) && (hCtrlWnd == hFromWnd))
				{
					OnComboBoxChange();
				}
			}

			break;
		}
	}

	return bReturn;
}

// PreSubclassWindow
void CXListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	// for Dialog based applications, this is a good place
	// to subclass the header control because the OnCreate()
	// function does not get called.
}

///////////////////////////////////////////////////////////////////////////////
// OnCreate
int CXListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return CListCtrl::OnCreate(lpCreateStruct);
}

///////////////////////////////////////////////////////////////////////////////
// OnCustomDraw
void CXListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;

	if ((GetItemCount() <= 0) || (GetColumnCount() <= 0))
	{
		return;
	}

	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT|CDDS_SUBITEM))
	{
		// This is the prepaint stage for a subitem. Here's where we set the
		// item's text and background colors. Our return value will tell
		// Windows to draw the subitem itself, but it will use the new colors
		// we set here.

		int nItem = static_cast<int> (pLVCD->nmcd.dwItemSpec);
		int nSubItem = pLVCD->iSubItem;

		if ((nItem == m_nEditItem) && (nSubItem == m_nEditSubItem))
		{
			*pResult = CDRF_SKIPDEFAULT;
			return;
		}

		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		pXLCD = GetXListCtrlData(nItem);
		ASSERT(pXLCD != NULL);

		COLORREF crText  = m_crWindowText;
		COLORREF crBkgnd = m_crWindow;

		if (pXLCD != NULL)
		{
			crText  = pXLCD->arSubItemData[nSubItem]->crText;
			crBkgnd = pXLCD->arSubItemData[nSubItem]->crBackground;

			if (!(pXLCD->bEnable) || !(pXLCD->arSubItemData[nSubItem]->bEnable))
				crText = m_crGrayText;
		}

		// store the colors back in the NMLVCUSTOMDRAW struct
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBkgnd;

		CRect rcSubItem(0,0,0,0);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);

		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcSubItem);

		if (rcSubItem.bottom > rcSubItem.top)
		{
			rcSubItem.bottom = rcSubItem.bottom - 1;
		}

		if ((pXLCD != NULL) && (pXLCD->arSubItemData[nSubItem]->bComboBoxEnable))
		{
//			if (GetItemState(nItem, LVIS_SELECTED))
				DrawComboBox(nItem, nSubItem, pDC, crText, crBkgnd, rcSubItem, pXLCD);
// 			else
// 				DrawText(nItem, nSubItem, pDC, crText, crBkgnd, rcSubItem, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else if ((pXLCD != NULL) && (pXLCD->arSubItemData[nSubItem]->bButtonEnable))
		{
			DrawButton(nItem, nSubItem, pDC, crText, crBkgnd, rcSubItem, pXLCD);
			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else if ((pXLCD != NULL) && (pXLCD->arSubItemData[nSubItem]->bCheckBoxEnable))
		{
			DrawCheckbox(nItem, nSubItem, pDC, crText, crBkgnd, rcSubItem, pXLCD);
			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else
		{
			rcSubItem.left += DrawImage(nItem, nSubItem, pDC, crText, crBkgnd, rcSubItem, pXLCD);
			DrawText(nItem, nSubItem, pDC, crText, crBkgnd, rcSubItem, pXLCD);
			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawComboBox
void CXListCtrl::DrawComboBox(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF crBkgnd,
							  CRect& rcSubItem,
							  XLISTCTRL_ITEM_DATA* pXLCD)
{
	UNUSED_ALWAYS(crText);
	UNUSED_ALWAYS(crBkgnd);

	ASSERT(pDC);
	ASSERT(pXLCD);

	// draw border
	int nSavedDC = pDC->SaveDC();
	CPen pen(PS_SOLID, 1, m_crBtnShadow);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CPen blackpen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&blackpen);

	// Fill interior with white
	CRect InteriorRect;
	InteriorRect = rcSubItem;
	InteriorRect.DeflateRect(1, 1);
	pDC->FillSolidRect(InteriorRect, RGB(255,255,255));

	CString strText = _T("");
	strText = GetItemText(nItem, nSubItem);
	if (!strText.IsEmpty())
	{
		// draw text
		CRect rcText(0,0,0,0);

		rcText = rcSubItem;
		rcText.DeflateRect(2,1,1,1);

		if (pXLCD->arSubItemData[0]->bEnable)
		{
			crText = m_crWindowText;
		}
		else
		{
			crText = m_crGrayText;
		}

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(m_crWindow);
		pDC->DrawText(strText, &rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	}

	if (pXLCD->arSubItemData[nSubItem]->bComboBoxEnable)
	{
		int nCXVScroll= 20;
		int nCYVScroll= 20;
		HTHEME hTheme = NULL;
		CRect rcArrow(0,0,0,0);

		nCXVScroll = ::GetSystemMetrics(SM_CXVSCROLL);
		nCYVScroll = ::GetSystemMetrics(SM_CYVSCROLL);

		rcArrow = rcSubItem;
		rcArrow.left = rcArrow.right - rcArrow.Height();
		rcArrow.DeflateRect(0,0,1,1);

		if (ThemeHelper.IsThemeLibAvailable())
		{
			hTheme = ThemeHelper.OpenThemeData(m_hWnd, _T("ComboBox"));
		}

		if (hTheme != NULL)
		{
			if (pXLCD->bEnable && pXLCD->arSubItemData[nSubItem]->bEnable)
			{
				ThemeHelper.DrawThemeBackground(hTheme, pDC->m_hDC, CP_DROPDOWNBUTTON, CBXS_NORMAL, &rcArrow, NULL);
			}
			else
			{
				ThemeHelper.DrawThemeBackground(hTheme, pDC->m_hDC, CP_DROPDOWNBUTTON, CBXS_DISABLED, &rcArrow, NULL);
			}

			ThemeHelper.CloseThemeData(hTheme);
		}
		else
		{
			pDC->DrawEdge(&rcSubItem, EDGE_RAISED, BF_RECT);
			pDC->FillSolidRect(rcSubItem, m_crBtnFace);
			pDC->DrawFrameControl(&rcArrow, DFC_SCROLL, DFCS_SCROLLCOMBOBOX);
		}
	}

	pDC->SelectObject(pOldPen);
	pDC->RestoreDC(nSavedDC);
}

///////////////////////////////////////////////////////////////////////////////
//
// DrawButton
void CXListCtrl::DrawButton(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF crBkgnd,
							  CRect& rcSubItem,
							  XLISTCTRL_ITEM_DATA* pXLCD)
{
	ASSERT(pDC != NULL);
	ASSERT(pXLCD != NULL);

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rcSubItem, crBkgnd);

	int nCXYCheckBox = 16;
	CRect rcButton(0,0,0,0);
	rcButton = rcSubItem;
	rcButton.left += 2;
	rcButton.top  += 1;
	rcButton.right -= 2;
	rcButton.bottom -= 1;

	// Fill rect around CheckBox with white
	pDC->FillSolidRect(&rcButton, m_crWindow);

	HTHEME hTheme = NULL;
	if (ThemeHelper.IsThemeLibAvailable())
	{
		hTheme = ThemeHelper.OpenThemeData(m_hWnd, _T("Button"));
	}

	if (hTheme != NULL)
	{
		int nOldBkMode = 0;
		nOldBkMode = pDC->SetBkMode(TRANSPARENT);
		if (pXLCD->arSubItemData[nSubItem]->bButtonEnable)
		{
			if (pXLCD->bEnable && pXLCD->arSubItemData[nSubItem]->bEnable)
			{
				ThemeHelper.DrawThemeBackground(hTheme, pDC->m_hDC, BP_PUSHBUTTON, PBS_NORMAL, &rcButton, NULL);
			}
			else
			{
				ThemeHelper.DrawThemeBackground(hTheme, pDC->m_hDC, BP_PUSHBUTTON, PBS_DISABLED, &rcButton, NULL);
			}
		}

		pDC->SetBkMode(nOldBkMode);
		ThemeHelper.CloseThemeData(hTheme);
	}
	else
	{
		if (pXLCD->arSubItemData[nSubItem]->bButtonEnable)
		{
			if (pXLCD->bEnable && pXLCD->arSubItemData[nSubItem]->bEnable)
			{
				pDC->DrawFrameControl(&rcButton, DFC_BUTTON, DFCS_BUTTONPUSH);
			}
			else
			{
				pDC->DrawFrameControl(&rcButton, DFC_BUTTON, DFCS_BUTTONPUSH);
			}
		}
	}

	CString strText = _T("");
	strText = GetItemText(nItem, nSubItem);
	if (!strText.IsEmpty())
	{
		CRect rcText(0,0,0,0);
		rcText = rcSubItem;

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		pDC->DrawText(strText, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawCheckbox
void CXListCtrl::DrawCheckbox(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF crBkgnd,
							  CRect& rcSubItem,
							  XLISTCTRL_ITEM_DATA* pXLCD)
{
	ASSERT(pDC != NULL);
	ASSERT(pXLCD != NULL);

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rcSubItem, crBkgnd);

	int nCXYCheckBox = 16;
	CRect rcCheckBox(0,0,0,0);
	rcCheckBox = rcSubItem;
	if (rcSubItem.Height() > 16)
	{
		rcCheckBox.top = rcCheckBox.top + (rcSubItem.Height() - nCXYCheckBox) / 2;
		rcCheckBox.left = rcCheckBox.left + 3;
		rcCheckBox.right = rcCheckBox.left + nCXYCheckBox;
		rcCheckBox.bottom = rcCheckBox.top + nCXYCheckBox;
	}
	else
	{
		rcCheckBox.left = rcCheckBox.left + 3;
		rcCheckBox.right = rcCheckBox.left + rcCheckBox.Height();
	}

	// Fill rect around CheckBox with white
	pDC->FillSolidRect(&rcCheckBox, m_crWindow);

	rcCheckBox.left += 1;

	HTHEME hTheme = NULL;

	if (ThemeHelper.IsThemeLibAvailable())
	{
		hTheme = ThemeHelper.OpenThemeData(m_hWnd, _T("Button"));
	}

	if (hTheme != NULL)
	{
		int nOldBkMode = 0;
		nOldBkMode = pDC->SetBkMode(TRANSPARENT);
		if (pXLCD->arSubItemData[nSubItem]->bCheckBoxEnable)
		{
			if (pXLCD->bEnable && pXLCD->arSubItemData[nSubItem]->bEnable)
			{
				if (pXLCD->arSubItemData[nSubItem]->nCheckedState == 1)
				{
					ThemeHelper.DrawThemeBackground(hTheme, pDC->m_hDC, BP_CHECKBOX, CBS_CHECKEDNORMAL, &rcCheckBox, NULL);
				}
				else
				{
					ThemeHelper.DrawThemeBackground(hTheme, pDC->m_hDC, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, &rcCheckBox, NULL);
				}
			}
			else
			{
				if (pXLCD->arSubItemData[nSubItem]->nCheckedState == 1)
				{
					ThemeHelper.DrawThemeBackground(hTheme, pDC->m_hDC, BP_CHECKBOX, CBS_CHECKEDDISABLED, &rcCheckBox, NULL);
				}
				else
				{
					ThemeHelper.DrawThemeBackground(hTheme, pDC->m_hDC, BP_CHECKBOX, CBS_UNCHECKEDDISABLED, &rcCheckBox, NULL);
				}
			}
		}

		pDC->SetBkMode(nOldBkMode);
		ThemeHelper.CloseThemeData(hTheme);
	}
	else
	{
		if (pXLCD->arSubItemData[nSubItem]->bCheckBoxEnable)
		{
			if (pXLCD->bEnable && pXLCD->arSubItemData[nSubItem]->bEnable)
			{
				if (pXLCD->arSubItemData[nSubItem]->nCheckedState == 1)
				{
					pDC->DrawFrameControl(&rcCheckBox, DFC_BUTTON, DFCS_BUTTONCHECK|DFCS_CHECKED);
				}
				else
				{
					pDC->DrawFrameControl(&rcCheckBox, DFC_BUTTON, DFCS_BUTTONCHECK);
				}
			}
			else
			{
				pDC->DrawFrameControl(&rcCheckBox, DFC_BUTTON, DFCS_BUTTONCHECK);
			}
		}
	}

	CString strText = _T("");
	strText = GetItemText(nItem, nSubItem);

	if (!strText.IsEmpty())
	{
		CRect rcText(0,0,0,0);

		rcText = rcSubItem;
		rcText.left = rcCheckBox.right + abs(rcCheckBox.left - rcText.left);

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		pDC->DrawText(strText, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetDrawColors
void CXListCtrl::GetDrawColors(int nItem,
							   int nSubItem,
							   COLORREF& colorText,
							   COLORREF& colorBkgnd)
{
	DWORD dwStyle    = GetStyle();
	DWORD dwExStyle  = GetExtendedStyle();

	COLORREF crText  = colorText;
	COLORREF crBkgnd = colorBkgnd;

	if (GetItemState(nItem, LVIS_SELECTED))
	{
		if (dwExStyle & LVS_EX_FULLROWSELECT)
		{
			// selected?  if so, draw highlight background
			crText  = m_crHighLightText;
			crBkgnd = m_crHighLight;

			// has focus?  if not, draw gray background
			if (m_hWnd != ::GetFocus())
			{
				if (dwStyle & LVS_SHOWSELALWAYS)
				{
					crText  = m_crWindowText;
					crBkgnd = m_crBtnFace;
				}
				else
				{
					crText  = colorText;
					crBkgnd = colorBkgnd;
				}
			}
		}
		else // not full row select
		{
			if (nSubItem == 0)
			{
				// selected?  if so, draw highlight background
				crText = m_crHighLightText;
				crBkgnd = m_crHighLight;

				// has focus?  if not, draw gray background
				if (m_hWnd != ::GetFocus())
				{
					if (dwStyle & LVS_SHOWSELALWAYS)
					{
						crText  = m_crWindowText;
						crBkgnd = m_crBtnFace;
					}
					else
					{
						crText  = colorText;
						crBkgnd = colorBkgnd;
					}
				}
			}
		}
	}

	colorText = crText;
	colorBkgnd = crBkgnd;
}

///////////////////////////////////////////////////////////////////////////////
// DrawImage
int CXListCtrl::DrawImage(int nItem,
						  int nSubItem,
						  CDC* pDC,
						  COLORREF crText,
						  COLORREF crBkgnd,
						  CRect rcSubItem,
						  XLISTCTRL_ITEM_DATA *pXLCD)
{
	ASSERT(pXLCD != NULL);
	if (pXLCD == NULL)
	{
		return 0;
	}

	int nImage = -1;
	int nImageCount = 0;
	CImageList* pImageList = NULL;

	nImage = pXLCD->arSubItemData[nSubItem]->nImage;
	if (nImage == -1)
	{
		return 0;
	}

	pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList == NULL)
	{
		return 0;
	}

	nImageCount = pImageList->GetImageCount();
	if (nImageCount < 0)
	{
		return 0;
	}

	if (nImage >= nImageCount)
	{
		return 0;
	}

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);
	pDC->FillSolidRect(&rcSubItem, crBkgnd);

	int nWidth = 0;
	SIZE sizeImage;
	IMAGEINFO info;

	sizeImage.cx = 0;
	sizeImage.cy = 0;

	if (pImageList->GetImageInfo(nImage, &info))
	{
		sizeImage.cx = info.rcImage.right - info.rcImage.left;
		sizeImage.cy = info.rcImage.bottom - info.rcImage.top;
	}

	if (rcSubItem.Width() > 0)
	{
		POINT point;

		point.y = rcSubItem.CenterPoint().y - (sizeImage.cy >> 1);
		point.x = rcSubItem.left;

		SIZE size;
		size.cx = rcSubItem.Width() < sizeImage.cx ? rcSubItem.Width() : sizeImage.cx;
		size.cy = rcSubItem.Height() < sizeImage.cy ? rcSubItem.Height() : sizeImage.cy;

		// save image list background color
		COLORREF rgb = pImageList->GetBkColor();

		// set image list background color
		pImageList->SetBkColor(crBkgnd);
		pImageList->DrawIndirect(pDC, nImage, point, size, CPoint(0, 0));
		pImageList->SetBkColor(rgb);
		nWidth = sizeImage.cx;
	}

	return nWidth;
}

///////////////////////////////////////////////////////////////////////////////
// DrawText
void CXListCtrl::DrawText(int nItem,
						  int nSubItem,
						  CDC *pDC,
						  COLORREF crText,
						  COLORREF crBkgnd,
						  CRect& rcSubItem,
						  XLISTCTRL_ITEM_DATA* pXLCD)
{
	ASSERT(pDC);
	ASSERT(pXLCD);

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rcSubItem, crBkgnd);

	rcSubItem.left += 2;

	CString str;
	str = GetItemText(nItem, nSubItem);

	if (!str.IsEmpty())
	{
		// get text justification
		HDITEM hditem;
		CHeaderCtrl* pHeaderCtrl = NULL;

		hditem.fmt = 0;
		pHeaderCtrl = GetHeaderCtrl();
		if (pHeaderCtrl != NULL)
		{
			hditem.mask = HDI_FORMAT;
			pHeaderCtrl->GetItem(nSubItem, &hditem);
		}

		int nJustifyFormat = hditem.fmt & HDF_JUSTIFYMASK;
		UINT nFormat = DT_VCENTER|DT_SINGLELINE;

		if (nJustifyFormat == HDF_CENTER)
			nFormat |= DT_CENTER;
		else if (nJustifyFormat == HDF_LEFT)
			nFormat |= DT_LEFT;
		else
			nFormat |= DT_RIGHT;

		int nSaveDC = pDC->SaveDC();
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		pDC->DrawText(str, &rcSubItem, nFormat);
		pDC->RestoreDC(nSaveDC);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetSubItemRect
BOOL CXListCtrl::GetSubItemRect(int nItem,
								int nSubItem,
								int nArea,
								CRect& rcSubItem)
{
	if (!CheckSubItemIndex(nItem, nSubItem))
	{
		return FALSE;
	}

	BOOL bRC = CListCtrl::GetSubItemRect(nItem, nSubItem, nArea, rcSubItem);

	if (nSubItem == 0)
	{
		if (GetColumnCount() > 1)
		{
			CRect rect1;
			bRC = GetSubItemRect(nItem, 1, LVIR_BOUNDS, rect1);
			rcSubItem.right = rect1.left;
		}
	}

	return bRC;
}

BOOL CXListCtrl::HitTestEx(CPoint& point, int& nItem, int& nSubItem, UINT* pFlags) const
{
	nSubItem = -1;
	nItem = CListCtrl::HitTest(point, pFlags);

	if (nItem < 0)
	{
		return FALSE;
	}

	// Make sure that the ListView is in LVS_REPORT
	if ((::GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
	{
		nSubItem = 0;
		return (nItem >= 0);
	}

	int nColumnWidth = 0;
	int nColumnIndex = 0;
	int nColumnCount = 0;
	CRect rcItem(0,0,0,0);
	CXListCtrl* pThisListCtrl = NULL;

	pThisListCtrl = const_cast<CXListCtrl*>(this);
	ASSERT(pThisListCtrl != NULL);
	if (pThisListCtrl != NULL)
	{
		nColumnCount = pThisListCtrl->GetColumnCount();
	}

	CListCtrl::GetItemRect(nItem, &rcItem, LVIR_BOUNDS);

	for (nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex)
	{
		nColumnWidth = CListCtrl::GetColumnWidth(nColumnIndex);
		rcItem.right = rcItem.left + nColumnWidth;

		if (rcItem.PtInRect(point))
		{
			nSubItem = nColumnIndex;
			return TRUE;
		}

		rcItem.OffsetRect(nColumnWidth, 0);
	}

	return FALSE;
}

void CXListCtrl::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	CListCtrl::OnNcMouseMove(nHitTest, point);
}

void CXListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CListCtrl::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CXListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	CListCtrl::OnLButtonDown(nFlags, point);

	int nItem = -1;
	int nSubItem = -1;

	HitTestEx(point, nItem, nSubItem);

	if (nItem >= 0)
	{
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		pXLCD = GetXListCtrlData(nItem);
		ASSERT(pXLCD != NULL);
		if (pXLCD == NULL)
		{
			return;
		}

		if (!(pXLCD->bEnable))
			return;

		if (!(pXLCD->arSubItemData[nSubItem]->bEnable))
			return;

		if (nSubItem >= 0)
		{
			if (pXLCD->arSubItemData[nSubItem]->bEditCtrlEnable)
			{
				ShowEditCtrl(nItem, nSubItem);
			}
			else if (pXLCD->arSubItemData[nSubItem]->bCheckBoxEnable)
			{
				int nChecked = pXLCD->arSubItemData[nSubItem]->nCheckedState;
				nChecked = (nChecked == 0) ? 1 : 0;
				pXLCD->arSubItemData[nSubItem]->nCheckedState = nChecked;
				UpdateItem(nItem, nSubItem);
			}
			else if (pXLCD->arSubItemData[nSubItem]->bComboBoxEnable)
			{
				ShowComboBox(nItem, nSubItem);
			}
			else if (pXLCD->arSubItemData[nSubItem]->bButtonEnable)
			{
				if (IsWindow(this->GetParent()->m_hWnd))
					::PostMessage(this->GetParent()->m_hWnd,DF_LIST_MESSAGE_CLICK_BUTTON , (WPARAM)nItem, (LPARAM)nSubItem);  //Send Message to Parent
			}
		}
	}
}

void CXListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		CListCtrl::OnLButtonUp(nFlags, point);
	}
}

void CXListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnRButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXListCtrl::OnPaint()
{
	if (GetItemCount() > 0)
	{
		Default();
	}
	else
	{
		CListCtrl::OnPaint();

		CDC* pDC = GetDC();
		int nSavedDC = pDC->SaveDC();

		CRect rc;
		GetWindowRect(&rc);
		ScreenToClient(&rc);
		CHeaderCtrl* pHC = GetHeaderCtrl();
		if (pHC != NULL)
		{
			CRect rcH;
			pHC->GetItemRect(0, &rcH);
			rc.top += rcH.bottom;
		}
		rc.top += 10;

		CString strText = _T(" ");
		COLORREF crText = m_crWindowText;
		COLORREF crBkgnd = m_crWindow;
		CBrush brush(crBkgnd);

		pDC->FillRect(rc, &brush);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		pDC->SelectStockObject(ANSI_VAR_FONT);
		pDC->DrawText(strText, -1, rc, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);
		pDC->RestoreDC(nSavedDC);

		ReleaseDC(pDC);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnSysColorChange
void CXListCtrl::OnSysColorChange()
{
	TRACE(_T("in CXListCtrl::OnSysColorChange\n"));

	CListCtrl::OnSysColorChange();

	m_cr3DFace        = ::GetSysColor(COLOR_3DFACE);
	m_cr3DHighLight   = ::GetSysColor(COLOR_3DHIGHLIGHT);
	m_cr3DShadow      = ::GetSysColor(COLOR_3DSHADOW);
	m_crBtnFace       = ::GetSysColor(COLOR_BTNFACE);
	m_crBtnShadow     = ::GetSysColor(COLOR_BTNSHADOW);
	m_crBtnText       = ::GetSysColor(COLOR_BTNTEXT);
	m_crGrayText      = ::GetSysColor(COLOR_GRAYTEXT);
	m_crHighLight     = ::GetSysColor(COLOR_HIGHLIGHT);
	m_crHighLightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_crWindow        = ::GetSysColor(COLOR_WINDOW);
	m_crWindowText    = ::GetSysColor(COLOR_WINDOWTEXT);
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CXListCtrl::OnDestroy()
{
	HideAllCtrls();
	CListCtrl::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////////
// SetCheckbox
BOOL CXListCtrl::SetCheckState(int nItem, int nSubItem, int nCheckedState)
{
	if (!CheckSubItemIndex(nItem, nSubItem))
	{
		return FALSE;
	}

	XLISTCTRL_ITEM_DATA* pXLCD = NULL;
	pXLCD = GetXListCtrlData(nItem);
	if (pXLCD == NULL)
	{
		return FALSE;
	}

	pXLCD->arSubItemData[nSubItem]->nCheckedState = nCheckedState;
	UpdateItem(nItem, nSubItem);
	return TRUE;
}

void CXListCtrl::EnableEditCtrl(int nItem, int nSubItem, BOOL bEnable, EditType editType)
{
	if (CheckSubItemIndex(nItem, nSubItem))
	{
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		pXLCD = GetXListCtrlData(nItem);
		if (pXLCD != NULL)
		{
			pXLCD->arSubItemData[nSubItem]->bEditCtrlEnable = bEnable;
			pXLCD->arSubItemData[nSubItem]->uiEditType = editType;
			UpdateItem(nItem, nSubItem);
		}
	}
}

void CXListCtrl::EnableCheckBox(int nItem, int nSubItem, BOOL bEnable)
{
	if (CheckSubItemIndex(nItem, nSubItem))
	{
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		pXLCD = GetXListCtrlData(nItem);
		if (pXLCD != NULL)
		{
			pXLCD->arSubItemData[nSubItem]->bCheckBoxEnable = bEnable;
			UpdateItem(nItem, nSubItem);
		}
	}
}

int CXListCtrl::GetCheckState(int nItem, int nSubItem)
{
	if (!CheckSubItemIndex(nItem, nSubItem))
	{
		return -1;
	}

	XLISTCTRL_ITEM_DATA* pXLCD = NULL;
	pXLCD = GetXListCtrlData(nItem);
	if (pXLCD == NULL)
	{
		return -1;
	}

	return pXLCD->arSubItemData[nSubItem]->nCheckedState;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CXListCtrl::IsItemEnabled(int nItem)
{
	if (!CheckItemIndex(nItem))
	{
		return FALSE;
	}

	XLISTCTRL_ITEM_DATA* pXLCD = NULL;
	pXLCD = GetXListCtrlData(nItem);
	if (pXLCD == NULL)
	{
		return FALSE;
	}

	return pXLCD->bEnable;
}

void CXListCtrl::EnableItem(int nItem, BOOL bEnable)
{
	if (CheckItemIndex(nItem))
	{
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		pXLCD = GetXListCtrlData(nItem);
		if (pXLCD != NULL)
		{
			pXLCD->bEnable = bEnable;
			UpdateItem(nItem, -1);
		}
	}
}

BOOL CXListCtrl::IsSubItemEnabled(int nItem, int nSubItem)
{
	if (!CheckSubItemIndex(nItem, nSubItem))
	{
		return FALSE;
	}

	XLISTCTRL_ITEM_DATA* pXLCD = NULL;
	pXLCD = GetXListCtrlData(nItem);
	if (pXLCD == NULL)
	{
		return FALSE;
	}

	return pXLCD->arSubItemData[nSubItem]->bEnable;
}

void CXListCtrl::EnableSubItem(int nItem, int nSubItem, BOOL bEnable)
{
	if (CheckSubItemIndex(nItem, nSubItem))
	{
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		pXLCD = GetXListCtrlData(nItem);
		if (pXLCD != NULL)
		{
			pXLCD->arSubItemData[nSubItem]->bEnable = bEnable;
			UpdateItem(nItem, nSubItem);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// SetComboBox
//
// Note:  SetItemText may also be used to set the initial combo selection.
//
void CXListCtrl::EnableComboBox(int nItem, int nSubItem, BOOL bEnable)
{
	if (CheckSubItemIndex(nItem, nSubItem))
	{
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		pXLCD = GetXListCtrlData(nItem);
		if (pXLCD != NULL)
		{
			pXLCD->arSubItemData[nSubItem]->bComboBoxEnable = bEnable;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CXListCtrl::EnableButton(int nItem, int nSubItem, BOOL bEnable)
{
	if (CheckSubItemIndex(nItem, nSubItem))
	{
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		pXLCD = GetXListCtrlData(nItem);
		if (pXLCD != NULL)
		{
			pXLCD->arSubItemData[nSubItem]->bButtonEnable = bEnable;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// UpdateItem
void CXListCtrl::UpdateItem(int nItem, int nSubItem)
{
	if (nItem >= 0)
	{
		CRect rcUpdate(0,0,0,0);

		if (nSubItem >= 0)
		{
			GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcUpdate);
		}
		else
		{
			GetItemRect(nItem, &rcUpdate, LVIR_BOUNDS);
		}

		InvalidateRect(&rcUpdate, FALSE);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetColumnCount
int CXListCtrl::GetColumnCount()
{
	CHeaderCtrl* pHeaderCtrl = NULL;
	pHeaderCtrl = GetHeaderCtrl();
	if (pHeaderCtrl != NULL)
	{
		return pHeaderCtrl->GetItemCount();
	}

	ASSERT(FALSE);
	return -1;
}

int CXListCtrl::GetSelectedItem()
{
	POSITION iPosition = NULL;
	iPosition = GetFirstSelectedItemPosition();
	if (iPosition != NULL)
	{
		return GetNextSelectedItem(iPosition);
	}

	return -1;
}

void CXListCtrl::SelectItem(int nItem, BOOL bSelected)
{
	UINT nMask = 0;
	UINT nState = 0;

	if (nItem >= 0)
	{
		nMask = LVIS_SELECTED|LVIS_FOCUSED;
		nState = (bSelected) ? (LVIS_SELECTED|LVIS_FOCUSED) : (0);
		SetItemState(nItem, nState, nMask);
	}
}

void CXListCtrl::DeselectAllItems()
{
	int nItem = 0;
	UINT nMask = 0;
	POSITION iPosition = NULL;

	nMask = LVIS_SELECTED|LVIS_FOCUSED;
	iPosition = GetFirstSelectedItemPosition();
	while (iPosition != NULL)
	{
		nItem = GetNextSelectedItem(iPosition);
		SetItemState(nItem, 0, nMask);
	}
}

LRESULT CXListCtrl::OnSetColumn(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
	return Default();
}

LRESULT CXListCtrl::OnInsertColumn(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;

	LRESULT lResult = 0;
	lResult = Default();

	if (lResult == 0)
	{
		int nItem = 0;
		int nItemCount = 0;
		int nColumnIndex = 0;
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		XLISTCTRL_SUBITEM_DATA* pSubData = NULL;

		nItemCount = GetItemCount();
		nColumnIndex = static_cast<int>(wParam);

		for (nItem = 0; nItem < nItemCount; ++nItem)
		{
			pXLCD = GetXListCtrlData(nItem);
			ASSERT(pXLCD != NULL);
			if (pXLCD != NULL)
			{
				pSubData = new XLISTCTRL_SUBITEM_DATA;
				pXLCD->arSubItemData.InsertAt(nColumnIndex, pSubData);
			}
		}
	}

	return lResult;
}

void CXListCtrl::OnInsertItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW lpNMListView = (LPNMLISTVIEW)pNMHDR;
	if ((lpNMListView != NULL) && (lpNMListView->iItem >= 0))
	{
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;

		try
		{
			pXLCD = new XLISTCTRL_ITEM_DATA(GetColumnCount());
			if (lpNMListView->iItem == m_arItemData.GetCount())
			{
				m_arItemData.Add(pXLCD);
			}
			else
			{
				m_arItemData.InsertAt(lpNMListView->iItem, pXLCD);
			}
		}
		catch (...)
		{
			pXLCD = NULL;
		}

		*pResult = (pXLCD == NULL) ? 1 : 0;
	}
}

void CXListCtrl::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW lpNMListView = (LPNMLISTVIEW)pNMHDR;
	if (lpNMListView->iItem >= 0)
	{
		XLISTCTRL_ITEM_DATA* pXLCD = NULL;
		ASSERT(lpNMListView->iItem < m_arItemData.GetCount());
		pXLCD = GetXListCtrlData(lpNMListView->iItem);
		m_arItemData.RemoveAt(lpNMListView->iItem);
		delete pXLCD;
		*pResult = 0;
	}
}

void CXListCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW lpNMListView = (LPNMLISTVIEW)pNMHDR;
	lpNMListView;
	*pResult = 0;
}

void CXListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW lpNMListView = (LPNMLISTVIEW)pNMHDR;
	lpNMListView;
	*pResult = 0;
}
