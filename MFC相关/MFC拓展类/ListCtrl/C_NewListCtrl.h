#pragma once
#define IDC_EDIT	1001
#define IDC_COMBOX	1002
//Message
#include "InnerWindows/DF_WM_USER.h"
#include "GL_User_Def.h"
// CNewListCtrl
class C_NewListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(C_NewListCtrl)

public:
	C_NewListCtrl();
	virtual ~C_NewListCtrl();
	
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnItemSelectChange(NMHDR *pNMHDR, LRESULT *pResult);
private:
	int m_nType;
	int m_nHitItem;
	int m_nHitSubItem; 

//Edit
private:
	CEdit m_edit; 
public:
	CEdit* GetEditCtrl();
	void ShowEdit(bool bShow, int nItem, int nSubItem, CRect rcCtrl);
	void FinishEdit();
	afx_msg void OnEnKillFocusEdit();

//Combox
private:
	CComboBox m_combox;
public:
	void ShowCombox(bool bShow, int nItem, int nSubItem, CRect rcCtrl);
	void FinishSel();
	int  GetTextIndex(CString strCurText);
	afx_msg void OnEnKillFocusCombox();

//Type
public:
	inline void SetType(int nTypE)
	{
		m_nType = nTypE;
	}

//Other
	void CheckEditData(int nHitSubItem, CString& strDataText);
	void SelectAllItem(bool bCheck);
	bool GetSelectAllState();
	void SetAllItemType(CString strType);
};


