// VidCapDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CVidCapDlg dialog
class CVidCapDlg : public CDialog
{
	// Construction
public:
	CVidCapDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	enum { IDD = IDD_VIDCAP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	UINT_PTR m_nTimer;
	CStatic m_PrvStatic;
	CStatic m_CapImgStatic;
	CComboBox m_AdapterCombo;
	CEdit m_editTimerInterval;
	UINT m_nTimerInterval;
	CButton m_RunButton;
	CStatic m_VideoFormat;                
	bool m_TakeSnapshot;


	CLSID pBmpEncoder;
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

	void DrawData(Gdiplus::Bitmap* pBitmap);        


	afx_msg LRESULT OnGraphMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedEnumadaptorsButton();
	afx_msg void OnBnClickedRunButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedFdetectCheck();       
	afx_msg void OnStnDblclickCapimgStatic();

};
