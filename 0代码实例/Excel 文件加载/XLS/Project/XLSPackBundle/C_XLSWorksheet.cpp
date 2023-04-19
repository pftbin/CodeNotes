#include "StdAfx.h"
#include "C_XLSWorksheet.h"
#include <math.h>

C_XLSWorksheet::C_XLSWorksheet(void)
{
	m_szSheetName=_T("");
}

C_XLSWorksheet::~C_XLSWorksheet(void)
{
}

STDMETHODIMP C_XLSWorksheet::GetValue(int nRow, int nColumn, CString& szValue)
{
	long lIndex[2];
	lIndex[0] = nRow;
	lIndex[1] = nColumn;

	szValue.Empty();

	VARIANT varValue;
	m_OLESafeArray.GetElement(lIndex, &varValue);

	switch(varValue.vt)
	{
	case VT_R8:
		{
// 			float lValue = varValue.dblVal;
// 			szValue.Format(_T("%0.2f"), lValue);
			ULONGLONG lValue = varValue.dblVal;
			double fValue = varValue.dblVal;
			if(fValue-lValue < 0.0001 && fValue-lValue >=0)
				szValue.Format(_T("%lld"), lValue);
			else
				szValue.Format(_T("%0.4f"), fValue);
			break;
		}
	case VT_BSTR:
		{
			szValue = (CString)varValue.bstrVal;
			break;
		}
	default:
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP C_XLSWorksheet::GetName(CString& szName)
{
	szName=m_szSheetName;
	return S_OK;
}

BOOL C_XLSWorksheet::Load(LPDISPATCH pWorksheetExcel)
{
	_Worksheet WorksheetExcel = pWorksheetExcel;

	m_szSheetName=WorksheetExcel.GetName();

	//Get the range object for which you wish to retrieve the
	//data and then retrieve the data (as a variant array, retValue).
	Range RangeExcel = WorksheetExcel.GetUsedRange();

	COleVariant covOptional(DISP_E_PARAMNOTFOUND,VT_ERROR);
	VARIANT retValue = RangeExcel.GetValue(covOptional);

	if(retValue.vt == VT_EMPTY) 
		return FALSE;
	m_OLESafeArray = retValue;

	//Create the SAFEARRAY from the VARIANT retValue.
	COleSafeArray oleSafeAry(retValue);

	//Determine the array's dimensions.
	long lNumRows;
	long lNumCols;
	oleSafeAry.GetUBound(1, &lNumRows);
	oleSafeAry.GetUBound(2, &lNumCols);

	//Display the elements in the SAFEARRAY.
	long index[2];
	VARIANT val;
	TRACE(_T("Contents of SafeArray\n"));
	TRACE(_T("=====================\n\t"));
	for(int colIndex = 1;colIndex <= lNumCols;colIndex++)
	{
		TRACE(_T("\t\tCol %d"), colIndex);
	}
	TRACE(_T("\n"));
	for(int rowIndex = 1;rowIndex <= lNumRows;rowIndex++)
	{
		TRACE(_T("Row %d"), rowIndex);
		for(int colIndex = 1;colIndex <= lNumCols;colIndex++)
		{
			index[0] = rowIndex;
			index[1] = colIndex;
			oleSafeAry.GetElement(index, &val);
			switch(val.vt)
			{
			case VT_R8:
				{
					TRACE(_T("\t\t%1.2f"), val.dblVal);
					break;
				}
			case VT_BSTR:
				{
					TRACE(_T("\t\t%s"),(CString)val.bstrVal);
					break;
				}

			case VT_EMPTY:
				{
					TRACE(_T("\t\t<empty>"));
					break;
				}
			}
		}
		TRACE(_T("\n"));
	}

	return TRUE;
}

STDMETHODIMP_(int) C_XLSWorksheet::GetRowCount()
{
	long lRowCount;
	m_OLESafeArray.GetUBound(1, &lRowCount);

	return lRowCount;
}

STDMETHODIMP_(int) C_XLSWorksheet::GetColumnCount()
{
	long lColCount;
	m_OLESafeArray.GetUBound(2, &lColCount);

	return lColCount;
}