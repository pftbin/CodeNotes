#pragma once


// C_BurNumEdit

class C_BurNumEdit : public CEdit
{
	DECLARE_DYNAMIC(C_BurNumEdit)

public:
	C_BurNumEdit();
	virtual ~C_BurNumEdit();

	enum{
		eNum_UInt = 0,
		eNum_Int,
		eNum_float,
		eNum_NA_Int,
		eNum_Text,
	};

	void SetNumType(int nType );
	void EnableReadOnly(BOOL bEnable);
	void SetMaxMinInt2(int iMin, int iMax);			//Int
	void SetMaxMinInt(DWORD dwMin, DWORD dwMax);	//UInt
	void SetMaxMinFloat(double dMin, double dMax);
	BOOL IsCharValid(UINT uChar);

	DWORD GetMaxInt();
	DWORD GetMinInt();
	double GetMaxFloat();
	double GetMinFloat();
protected:
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKillFocus(CWnd* pOldWnd);

protected:

	int  m_nNumType;
	BOOL m_bReadOnly;

	DWORD m_dwMax;
	DWORD m_dwMin;

	double m_dMax;
	double m_dMin;

	int m_iMax;
	int m_iMin;

	CString m_strLastText;

	BOOL m_bShowMessage;
	BOOL m_bIsEditByUser;
public:
	BOOL IsEdited();
};


