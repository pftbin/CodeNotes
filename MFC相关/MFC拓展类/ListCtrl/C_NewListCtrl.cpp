// CNewListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "C_NewListCtrl.h"


// CNewListCtrl

IMPLEMENT_DYNAMIC(C_NewListCtrl, CListCtrl)

C_NewListCtrl::C_NewListCtrl()
{
	m_nType = 0;
	m_nHitItem = -1;
	m_nHitSubItem = -1;
}

C_NewListCtrl::~C_NewListCtrl()
{
}

BEGIN_MESSAGE_MAP(C_NewListCtrl, CListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED,OnItemSelectChange)
	ON_EN_KILLFOCUS(IDC_EDIT, OnEnKillFocusEdit)
	ON_CBN_KILLFOCUS(IDC_COMBOX,OnEnKillFocusCombox)
END_MESSAGE_MAP()

// CNewListCtrl message handlers
BOOL C_NewListCtrl::PreTranslateMessage(MSG* pMsg)
{
	//Block ESC
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	//Block Enter
	else if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)) 
	{
		return TRUE;
	}
	else
	{
		return CListCtrl::PreTranslateMessage(pMsg);
	}
	return TRUE;
}

afx_msg void C_NewListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rcListCtrl;
	LVHITTESTINFO lvhti;
	lvhti.pt = point;
	m_nHitItem = SubItemHitTest(&lvhti);
	if (m_nHitItem == -1) 
		return;
	m_nHitSubItem = lvhti.iSubItem;
	GetSubItemRect(m_nHitItem, m_nHitSubItem, LVIR_LABEL, rcListCtrl);
	if (m_nType == 1)
	{
		if (lvhti.iSubItem > 1 && lvhti.iItem != 0)
			ShowEdit(TRUE, m_nHitItem, m_nHitSubItem, rcListCtrl);
		if (lvhti.iSubItem  == 1)
			ShowCombox(TRUE,m_nHitItem, m_nHitSubItem, rcListCtrl);
		if (lvhti.iSubItem == 0)
		{
// 			int nItemIndex = lvhti.iItem;
// 			if (nItemIndex != -1)
// 			{
// 				bool bChexk = GetCheck(nItemIndex);
// 				SetCheck(nItemIndex,!bChexk);
// 			}
// 			bool  bAllChecked = GetSelectAllState();
// 			if (IsWindow(this->GetParent()->m_hWnd))
// 				::SendMessage(this->GetParent()->m_hWnd,WM_USER_UPDATE_LIST_SETCHECKBOX,NULL,(LPARAM)bAllChecked);  //Send Message to Parent
		}
	}
	else if (m_nType == 0)
	{
		if (lvhti.iSubItem > 0)
			ShowEdit(TRUE, m_nHitItem, m_nHitSubItem, rcListCtrl);
	}

// 		if (IsWindow(this->GetParent()->m_hWnd))
// 			::SendMessage(this->GetParent()->m_hWnd,NM_DBLCLK,NULL,NULL);  //Send NM_DBLCLK to Parent	
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

afx_msg void C_NewListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	ShowEdit(false, -1, -1, CRect());
	ShowCombox(false,-1,-1,CRect());

	CListCtrl::OnLButtonDown(nFlags,point);

// 	LVHITTESTINFO lvhti;
// 	lvhti.pt = point;
// 	SubItemHitTest(&lvhti);
// 	if (lvhti.flags == 8) //At CheckBox
// 	{
// 		bool  bAllChecked = GetSelectAllState();
// 		if (IsWindow(this->GetParent()->m_hWnd))
// 			::SendMessage(this->GetParent()->m_hWnd,WM_USER_UPDATE_LIST_SETCHECKBOX,NULL,(LPARAM)bAllChecked);  //Send Message to Parent
// 	}	
}

afx_msg void C_NewListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonUp(nFlags,point);

	if (IsWindow(this->GetParent()->m_hWnd))
		::SendMessage(this->GetParent()->m_hWnd,NM_CLICK,NULL,NULL);  //Send NM_CLICK to Parent
}

afx_msg void C_NewListCtrl::OnItemSelectChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
	ShowEdit(false, -1, -1, CRect());
	ShowCombox(false,-1,-1,CRect());

	return;
}
//Edit
void C_NewListCtrl::ShowEdit(bool bShow, int nItem, int nSubItem, CRect rcCtrl)
{
	if (nItem == -1 || nSubItem == -1 || bShow == false)
	{
		if (m_edit.m_hWnd != NULL)
		{
			FinishEdit();
			m_edit.ShowWindow(SW_HIDE);
		}
		return;
	}
	if (m_edit.m_hWnd == NULL)
	{
		m_edit.Create(ES_AUTOHSCROLL | WS_CHILD | ES_LEFT | ES_WANTRETURN | WS_BORDER,CRect(0,0,0,0), this, IDC_EDIT);
		m_edit.ModifyStyle(0,ES_NUMBER,TRUE); //Only Number
		m_edit.ShowWindow(SW_HIDE);

		CFont tpFont;
		tpFont.CreateStockObject(DEFAULT_GUI_FONT);
		m_edit.SetFont(&tpFont);
		tpFont.DeleteObject();
	}

	if (bShow == TRUE)
	{
		CString cstrItem = GetItemText(nItem, nSubItem);
		m_edit.MoveWindow(&rcCtrl);
		m_edit.ShowWindow(SW_SHOW);
		m_edit.SetWindowText(cstrItem);
		m_edit.SetFocus();
		m_edit.SetSel(-1); 
	}
	else
		m_edit.ShowWindow(SW_HIDE);
}

afx_msg void C_NewListCtrl::OnEnKillFocusEdit()
{
	FinishEdit();
	ShowEdit(false, -1, -1, CRect());
// 	if (IsWindow(this->GetParent()->m_hWnd))
// 		::SendMessage(this->GetParent()->m_hWnd,WM_USER_EDIT_END,NULL,NULL);  //Send EndEdit to Parent
}

void C_NewListCtrl::FinishEdit()
{
	switch(m_nType)
	{
		case 0:
			if (m_nHitSubItem > 0 )
			{
				CString strText;
				m_edit.GetWindowText(strText);
				CheckEditData(m_nHitSubItem,strText);
				SetItemText(m_nHitItem,m_nHitSubItem,strText);
			}
			break;
		case 1:
			if (m_nHitSubItem > 1 )
			{
				CString strText;
				m_edit.GetWindowText(strText);
				CheckEditData(m_nHitSubItem,strText);
				SetItemText(m_nHitItem,m_nHitSubItem,strText);
			}
			break;
		default:
			break;
	}
}
//Combox
void C_NewListCtrl::ShowCombox(bool bShow, int nItem, int nSubItem, CRect rcCtrl)
{
	if (nItem == -1 || nSubItem == -1 || bShow == false)
	{
		if (m_combox.m_hWnd != NULL)
		{
			FinishSel();
			m_combox.ShowWindow(SW_HIDE);
		}
		return;
	}
	if (m_combox.m_hWnd == NULL)
	{
//		m_combox.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_OEMCONVERT, CRect(0, 0, 0, 0), this, IDC_COMBOX);
		m_combox.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_HASSTRINGS | WS_VSCROLL, CRect(0, 0, 0, 0), this, IDC_COMBOX);
		m_combox.AddString(strDataType[0]);
		m_combox.AddString(strDataType[1]);
		m_combox.AddString(strDataType[2]);

		CString strCur;
		strCur = GetItemText(m_nHitItem,m_nHitSubItem);
		int nCurIndex = GetTextIndex(strCur);
		m_combox.SetCurSel(nCurIndex);

		m_combox.ShowWindow(SW_SHOW);

		CFont tpFont;
		tpFont.CreateStockObject(DEFAULT_GUI_FONT);
		m_combox.SetFont(&tpFont);
		tpFont.DeleteObject();
	}

	if (bShow == TRUE)
	{
		CString cstrItem = GetItemText(nItem, nSubItem);
		m_combox.MoveWindow(&rcCtrl);

		int nItemCount = m_combox.GetCount();
		if (nItemCount != 3)
		{
			m_combox.ResetContent();
			m_combox.AddString(strDataType[0]);
			m_combox.AddString(strDataType[1]);
			m_combox.AddString(strDataType[2]);
		}
		CString strCur;
		strCur = GetItemText(m_nHitItem,m_nHitSubItem);
		int nCurIndex = GetTextIndex(strCur);
		m_combox.SetCurSel(nCurIndex);

		m_combox.ShowWindow(SW_SHOW);
	}
	else
		m_combox.ShowWindow(SW_HIDE);
}

int  C_NewListCtrl::GetTextIndex(CString strCurText)
{
	int nCBIndex = 0;
	if (strCurText.CompareNoCase(strDataType[0]) == 0)
	{
		nCBIndex = 0;
	}
	else if (strCurText.CompareNoCase(strDataType[1]) == 0)
	{
		nCBIndex = 1;
	}
	else if (strCurText.CompareNoCase(strDataType[2]) == 0)
	{
		nCBIndex = 2;
	}

	return nCBIndex;
}

afx_msg void C_NewListCtrl::OnEnKillFocusCombox()
{
	FinishSel();
	ShowCombox(false,-1,-1,CRect());
}

void C_NewListCtrl::FinishSel()
{
	if (m_nHitSubItem == 1)
	{
		CString strText;
		int nSelect = m_combox.GetCurSel();
		if (nSelect == -1)
		{
			nSelect = 0;
		}
		m_combox.GetLBText(nSelect,strText);
		SetItemText(m_nHitItem,m_nHitSubItem,strText);
		if (m_nHitItem == 0)
		{
			SetAllItemType(strText);
		}
	}
}

//Other
void C_NewListCtrl::SelectAllItem(bool bCheck)
{
	if (m_nType == 1)
	{
		int nItemCount = GetItemCount();
		if (nItemCount != -1 && nItemCount != 0)
		{
			for (int i = 0; i < nItemCount; i++)
			{
				SetCheck(i,bCheck);
			}
		}
	}
}

bool C_NewListCtrl::GetSelectAllState()
{
	bool  bAllChecked = true;
	int nItemCount = GetItemCount();
	for (int i = 0; i < nItemCount; i++)
	{
		if (GetCheck(i))
		{
			bAllChecked = true;
		}
		else
		{
			bAllChecked = false;
			break;
		}

	}
	return bAllChecked;
}

void C_NewListCtrl::CheckEditData(int nHitSubItem, CString& strDataText)
{
	if (m_nType == 1)
	{
		int nData = _ttoi(strDataText);
		switch(nHitSubItem)
		{
			case 0:break;
			case 1:break;
			case 2: 
				if(nData > 255)
					strDataText = _T("255");
				if(nData < 0)
					strDataText = _T("0");
				break;
			case 3:
				if(nData > 9100000)
					strDataText = _T("9100000");
				if(nData < 0)
					strDataText = _T("0");
				break;
			case 4: 
				if(nData > 3000)
					strDataText = _T("3000");
				if(nData < 1)
					strDataText = _T("1");
				break;
			case 5: 
				if(nData > 500000)
					strDataText = _T("500000");
				if(nData < 0)
					strDataText = _T("0");
				break;
			default: break;
		}
	}
}

void C_NewListCtrl::SetAllItemType(CString strType)
{
	if (m_nType == 1)
	{
		int nItemCnt = GetItemCount();
		for (int i = 0; i < nItemCnt; i++)
		{
			SetItemText(i,1,strType);
		}
	}
}

