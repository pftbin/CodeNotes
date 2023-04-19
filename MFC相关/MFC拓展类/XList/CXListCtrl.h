// XListCtrl.h  Version 1.3
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __OIDXLISTCTRL_H__
#define __OIDXLISTCTRL_H__


#define DF_LIST_MESSAGE_CLICK_BUTTON		(WM_USER+1000)

#define DF_MAX_ROW	100
#define DF_MAX_COL  100
#include <vector>
///////////////////////////////////////////////////////////////////////////////
// CXListCtrl data
struct XLISTCTRL_SUBITEM_DATA
{
	// ctor
	XLISTCTRL_SUBITEM_DATA()
	{
		bEnable       = TRUE;
		nImage        = -1;
		crText        = ::GetSysColor(COLOR_WINDOWTEXT);
		crBackground  = ::GetSysColor(COLOR_WINDOW);
		bEditCtrlEnable = FALSE;
		bComboBoxEnable = FALSE;
		bCheckBoxEnable = FALSE;
		bButtonEnable = FALSE;
		nCheckedState = -1;
	}

	BOOL bEnable;

	// For Image
	int nImage;

	// For Color
	COLORREF crText;
	COLORREF crBackground;

	// For EditCtrl
	BOOL bEditCtrlEnable;

	// For CheckBox
	BOOL bCheckBoxEnable;
	int nCheckedState;

	// For ComboBox
	BOOL bComboBoxEnable;

	//For Button
	BOOL bButtonEnable;
};

struct XLISTCTRL_ITEM_DATA
{
	XLISTCTRL_ITEM_DATA(int nColumnCount)
	{
		int nIndex = 0;
		XLISTCTRL_SUBITEM_DATA* pData = NULL;

		bEnable = TRUE;
		arSubItemData.SetSize(nColumnCount);
		for (nIndex = 0; nIndex < nColumnCount; ++nIndex)
		{
			pData = new XLISTCTRL_SUBITEM_DATA;
			arSubItemData.SetAt(nIndex, pData);
		}
	}

	~XLISTCTRL_ITEM_DATA()
	{
		INT_PTR nIndex = 0;
		INT_PTR nCount = 0;
		XLISTCTRL_SUBITEM_DATA* pData = NULL;

		nCount = arSubItemData.GetCount();
		for (nIndex = 0; nIndex < nCount; ++nIndex)
		{
			pData = arSubItemData.GetAt(nIndex);
			ASSERT(pData != NULL);
			delete pData;
		}

		arSubItemData.RemoveAll();
	}

	BOOL bEnable;
	CArray<XLISTCTRL_SUBITEM_DATA*> arSubItemData;
};

///////////////////////////////////////////////////////////////////////////////
// CXListCtrl class
class CXListCtrl : public CListCtrl
{
	DECLARE_DYNCREATE(CXListCtrl)

// Construction
public:
	CXListCtrl();
	virtual ~CXListCtrl();

// Attributes
public:

// Operations
	virtual int GetSelectedItem();
	virtual void SelectItem(int nItem, BOOL bSelected = TRUE);
	virtual void DeselectAllItems();

	virtual BOOL GetSubItemRect(int iItem, int iSubItem, int nArea, CRect& rcSubItem);
	virtual BOOL HitTestEx(CPoint& point, int& nItem, int& nSubItem, UINT* pFlags = NULL) const;

	virtual int GetColumnCount();

	virtual BOOL IsItemEnabled(int nItem);
	virtual void EnableItem(int nItem, BOOL bEnable);

	virtual BOOL IsSubItemEnabled(int nItem, int nSubItem);
	virtual void EnableSubItem(int nItem, int nSubItem, BOOL bEnable);

	virtual void EnableEditCtrl(int nItem, int nSubItem, BOOL bEnable);
	virtual void EnableCheckBox(int nItem, int nSubItem, BOOL bEnable);
	virtual void EnableComboBox(int nItem, int nSubItem, BOOL bEnable);
	virtual void EnableButton(int nItem, int nSubItem, BOOL bEnable);


	virtual int GetCheckState(int nItem, int nSubItem);
	virtual BOOL SetCheckState(int nItem, int nSubItem, int nCheckState);

	virtual void UpdateItem(int nItem, int nSubItem = -1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListCtrl)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnParentNotify(UINT message, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	virtual BOOL GetEditCtrlInfo(int iItem, int iSubItem, CString& strText);
	virtual BOOL ShowEditCtrl(int iItem, int iSubItem);
	virtual BOOL OnEditCtrlChange();
	virtual void HideEditCtrl();

	int GetIndexFromArray(CStringArray& arText, CString strString);
	virtual BOOL GetComboBoxInfo(int iItem, int iSubItem, CStringArray& arText, int& nSelectIndex);
	virtual BOOL ShowComboBox(int iItem, int iSubItem);
	virtual BOOL OnComboBoxChange();
	virtual void HideComboBox();

	// For EditCtrl and ComboBox
	virtual BOOL CheckEditInfo(int iItem, int iSubItem, LPCTSTR lpszNewText);
	virtual void UpdateEditInfo(int iItem, int iSubItem, LPCTSTR lpszNewText);
	virtual void HideAllCtrls();

	BOOL CheckItemIndex(int nItem);
	BOOL CheckSubItemIndex(int nItem, int nSubItem);
	XLISTCTRL_ITEM_DATA* GetXListCtrlData(int nItem);

	void DrawCheckbox(int nItem, 
					  int nSubItem, 
					  CDC *pDC, 
					  COLORREF crText,
					  COLORREF crBkgnd,
					  CRect& rcSubItem, 
					  XLISTCTRL_ITEM_DATA *pCLD);

	void DrawComboBox(int nItem, 
					  int nSubItem, 
					  CDC *pDC, 
					  COLORREF crText,
					  COLORREF crBkgnd,
					  CRect& rcSubItem, 
					  XLISTCTRL_ITEM_DATA *pCLD);
	void DrawButton(int nItem, 
					int nSubItem, 
					CDC *pDC, 
					COLORREF crText,
					COLORREF crBkgnd,
					CRect& rcSubItem, 
					XLISTCTRL_ITEM_DATA *pCLD);

	int DrawImage(int nItem, 
				  int nSubItem, 
				  CDC* pDC, 
				  COLORREF crText,
				  COLORREF crBkgnd,
				  CRect rcSubItem,
				  XLISTCTRL_ITEM_DATA *pXLCD);

	void DrawText(int nItem, 
				  int nSubItem, 
				  CDC *pDC, 
				  COLORREF crText,
				  COLORREF crBkgnd,
				  CRect& rcSubItem, 
				  XLISTCTRL_ITEM_DATA *pCLD);

	void GetDrawColors(int nItem,
					   int nSubItem,
					   COLORREF& colorText,
					   COLORREF& colorBkgnd);

	COLORREF		m_cr3DFace;
	COLORREF		m_cr3DHighLight;
	COLORREF		m_cr3DShadow;
	COLORREF		m_crBtnFace;
	COLORREF		m_crBtnShadow;
	COLORREF		m_crBtnText;
	COLORREF		m_crGrayText;
	COLORREF		m_crHighLight;
	COLORREF		m_crHighLightText;
	COLORREF		m_crWindow;
	COLORREF		m_crWindowText;

	int m_nEditItem;
	int m_nEditSubItem;
	BOOL m_bProcessEditCtrlChange;
	BOOL m_bProcessComboBoxChange;
	BOOL m_bProcessHideEditCtrl;
	BOOL m_bProcessHideComboBox;
	CEdit* m_pEditWnd;
	CComboBox* m_pComboBoxWnd;
	CArray<XLISTCTRL_ITEM_DATA*> m_arItemData;

	// Generated message map functions
protected:
	//{{AFX_MSG(CXListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysColorChange();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNcMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSetColumn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInsertColumn(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInsertItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	CStringArray m_comboBoxArray[DF_MAX_ROW][DF_MAX_COL];


public:
	BOOL SetComboBoxArrayInfo(CStringArray& arText);						 //every item same
	BOOL SetComboBoxArrayInfo(int iItem, int iSubItem, CStringArray& arText);//single item setting
	BOOL GetComboBoxArrayInfo(int iItem, int iSubItem, CStringArray& arText);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // __OIDXLISTCTRL_H__
