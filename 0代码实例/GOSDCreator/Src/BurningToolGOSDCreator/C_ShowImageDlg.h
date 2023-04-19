#pragma once


// C_ShowImageDlg dialog

class C_ShowImageDlg : public CDialog
{
	DECLARE_DYNAMIC(C_ShowImageDlg)
public:
	C_ShowImageDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~C_ShowImageDlg();
	void  SetImagePath(CString strImagePath);

	// Dialog Data
	enum { IDD = IDD_DIALOG_SHOW };
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	SCROLLINFO	m_scrollHinfo;
	SCROLLINFO	m_scrollVinfo;
	CString		m_strImagePath;
	HBITMAP		m_hBitmap;
	int			m_nImageW;
	int			m_nImageH;

	void ShowImage();
};
