#pragma once

#define  DF_TEXTCNT_MAT   125
#define  DF_FLOATCNT_MAX  6
class C_NewEdit : public CEdit
{
	DECLARE_DYNAMIC(C_NewEdit)
public:
	C_NewEdit(void);
	virtual~C_NewEdit(void);

	enum E_EditType
	{
		eEditType_ALL = 0,
		eEditType_UINT,
		eEditType_INT,
		eEditType_FLOAT,
		eEditType_HEX,

		eEditTypeCount
	};

	void SetEditType(E_EditType eEditType)
	{
		m_nEditType = eEditType;
	}

	void SetMinMax(double dbMin, double dbMax)
	{
		m_dbMinValue = dbMin;
		m_dbMaxValue = dbMax;
	}

	void SetFloatCount(UINT nCount)
	{
		if (nCount <= DF_FLOATCNT_MAX)
			m_nFloatCount = nCount;
	 }

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pWnd);

protected:
	UINT m_nEditType;
	double m_dbMinValue;
	double m_dbMaxValue;
	UINT m_nTextCount;	//default is 125
	UINT m_nFloatCount;	//default is 6
	
protected:
	BOOL IsValidChar(UINT nChar);
	int  Str2Int(CString strTextString,UINT uSysNum);
	double Str2Double(CString strTextString);

	BOOL CheckSymbol(UINT nChar);
	BOOL CheckOriginalText();
};

