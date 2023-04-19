#include "StdAfx.h"
#include "C_NewEdit.h"

IMPLEMENT_DYNAMIC(C_NewEdit, CEdit)
C_NewEdit::C_NewEdit(void)
	: m_nEditType(eEditType_ALL)
	, m_dbMinValue(0.00)
	, m_dbMaxValue(65535.00)
	, m_nTextCount(DF_TEXTCNT_MAT)
	, m_nFloatCount(DF_FLOATCNT_MAX)
{
}


C_NewEdit::~C_NewEdit(void)
{
}

BEGIN_MESSAGE_MAP(C_NewEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

BOOL C_NewEdit::IsValidChar(UINT nChar)
{
	if (nChar > 0 && nChar < 128)//ASCII 0-128
	{
		return TRUE;
	}

	return FALSE;
}

int  C_NewEdit::Str2Int(CString strTextString,UINT uSysNum)
{
	int nResult = 0;
	if (strTextString.GetLength() != 0)
	{
		char pBuf[MAX_PATH] = {0};
		USES_CONVERSION;
		strcpy_s(pBuf,T2A(strTextString));
		nResult = strtol(pBuf,NULL,uSysNum);
	}

	return nResult;
}

double C_NewEdit::Str2Double(CString strTextString)
{
	double dbResult = 0;
	if (strTextString.GetLength() != 0)
	{
		char pBuf[MAX_PATH] = {0};
		USES_CONVERSION;
		strcpy_s(pBuf,T2A(strTextString));
		dbResult = strtod(pBuf,NULL);
	}

	return dbResult;
}

BOOL C_NewEdit::CheckSymbol(UINT nChar)
{
	if ((nChar != '-')&&(nChar != '.'))
		return TRUE;

	CString strText;
	GetWindowText(strText);
	int nStart = -1,nEnd = m_nTextCount;
	GetSel(nStart,nEnd);

	if (m_nEditType == eEditType_INT)
	{
		if (nChar == '-')
		{
			if (!strText.IsEmpty())
				return FALSE;
		}
	}

	if (m_nEditType == eEditType_FLOAT)
	{
		if (nChar == '-')
		{
			if (!strText.IsEmpty())
				return FALSE;
		}
		if (nChar == '.')
		{
			//have '.'
			if (strText.Find(_T("."))>0)
				return FALSE;
			//no have '.'
			if (strText.IsEmpty())
				return FALSE;
			if (strText.GetLength() - nEnd > m_nFloatCount)
				return FALSE;
		}
	}

	return TRUE;
}

BOOL C_NewEdit::CheckOriginalText()
{
	CString strText;
	GetWindowText(strText);
	int nStart = -1,nEnd = m_nTextCount;
	GetSel(nStart,nEnd);

	if ((nEnd - nStart) == strText.GetLength())//full text select
		return TRUE;

	if (strText.GetLength() >= m_nTextCount)
		return FALSE;

	if (m_nEditType == eEditType_FLOAT)
	{
		//some text select
		if ((nEnd - nStart) > 0)
			return TRUE;

		//no text select
		int nLen = strText.GetLength();
		int nPos1 = strText.Find(_T("."));
		if (nPos1 > 0)
		{
			if (nStart <= nPos1)//change integer part
				return TRUE;
			else				//change decimals part
			{
				if (nLen - (nPos1+1) >= m_nFloatCount)
					return FALSE;
			}
		}
	}

	return TRUE;
}

// C_BurNumEdit message handlers
afx_msg void C_NewEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsValidChar(nChar))
	{
		//operator
		if (nChar == VK_BACK)
		{
			CEdit::OnChar(nChar,nRepCnt,nFlags);
		}
		//input
		else
		{
			if (m_nEditType == eEditType_ALL)
			{
				//do nothing
			}
			else if (m_nEditType == eEditType_UINT)
			{
				if ((nChar < '0' || nChar > '9'))
					return;
			}
			else if (m_nEditType == eEditType_INT)
			{
				if ((nChar < '0' || nChar > '9') && (nChar != 0x2D))//accept '-'
					return;	
				if(!CheckSymbol(nChar))
					return;
			}
			else if (m_nEditType == eEditType_FLOAT)
			{
				if ((nChar < '0' || nChar > '9') && (nChar != 0x2D) && (nChar != 0x2E))//accept '-' and '.'
					return;
				if(!CheckSymbol(nChar))
					return;
			}
			else if (m_nEditType == eEditType_HEX)
			{
				if ((nChar < '0' || nChar > '9') &&
					(nChar < 'A' || nChar > 'F') &&
					(nChar < 'a' || nChar > 'f') )
					return;
			}
			if (CheckOriginalText())
				CEdit::OnChar(nChar,nRepCnt,nFlags);
		}
	}
}

afx_msg void C_NewEdit::OnKillFocus(CWnd* pWnd)
{
	CString strMessage;
	CString strText;
	GetWindowText(strText);

	if (m_nEditType == eEditType_UINT || m_nEditType == eEditType_INT)
	{
		int nValue = Str2Int(strText,10);
		if ((nValue < (int)m_dbMinValue) || (nValue > (int)m_dbMaxValue))
		{
			strText.Format(_T("%d"),(int)m_dbMinValue);
			SetWindowText(strText);

			strMessage.Format(_T("Edit value must in %d - %d."),(int)m_dbMinValue,(int)m_dbMaxValue);
			AfxMessageBox(strMessage);
			return;
		}
	}
	if (m_nEditType == eEditType_FLOAT)
	{
		double dbValue = Str2Double(strText);
		if (dbValue < m_dbMinValue || dbValue > m_dbMaxValue)
		{
			strText.Format(_T("%.3f"),m_dbMinValue);
			SetWindowText(strText);
			
			strMessage.Format(_T("Edit value must in %.3f - %.3f."),m_dbMinValue,m_dbMaxValue);
			AfxMessageBox(strMessage);
			return;
		}
	}
	if (m_nEditType == eEditType_HEX)
	{
		UINT uiValue = Str2Int(strText,16);
		if ((uiValue < (UINT)m_dbMinValue) || uiValue > (UINT)m_dbMaxValue)
		{
			strText.Format(_T("%X"),(UINT)m_dbMinValue);
			SetWindowText(strText);

			strMessage.Format(_T("Edit value must in %X - %X."),(UINT)m_dbMinValue,(UINT)m_dbMaxValue);
			AfxMessageBox(strMessage);
			return;
		}
	}

	CEdit::OnKillFocus(pWnd);
}