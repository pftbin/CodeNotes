#pragma once
#include "afxwin.h"

#include "ColorButton.h"

// C_MyColorDlg dialog

class C_MyColorDlg : public CDialog
{
	DECLARE_DYNAMIC(C_MyColorDlg)

public:
	C_MyColorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~C_MyColorDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_COLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedColorPalette();
	afx_msg void OnBnClickedLum();
	afx_msg void OnEnChangeEditR();
	afx_msg void OnEnChangeEditG();
	afx_msg void OnEnChangeEditB();
	afx_msg void OnEnChangeEditY();
	afx_msg void OnEnChangeEditU();
	afx_msg void OnEnChangeEditV();

private:
	CPalette m_palSys;
	CMFCColorPickerCtrl m_wndLum;
	CMFCColorPickerCtrl m_wndColorPalette;
	CColorButton m_btnCurClr;

	COLORREF m_color;
	int m_nR;
	int m_nG;
	int m_nB;
	int m_nY;
	int m_nU;
	int m_nV;

	void UpdateUI();
	void UpdateCtrlColor();
	void UpdateEditRGB();
	void UpdateEditYUV();

	void GetYUV(COLORREF clr,int* nY,int* nU,int* nV);
	COLORREF GetClrByYUV(int nY,int nU,int nV);

public:
	void InitByColor(COLORREF clr);
	COLORREF GetColor();
	
	void InitByYUV(int nY,int nU,int nV);
	void GetYUV(int* nY,int* nU,int* nV);
};
