#include "StdAfx.h"
#include "C_XLSWorksheetWrite.h"


C_XLSWorksheetWrite::C_XLSWorksheetWrite(LPDISPATCH pWorksheetExcel, LPCTSTR pszSheetName)
{
	m_WorksheetExcel = pWorksheetExcel;

	m_WorksheetExcel.SetName(pszSheetName);
	m_szSheetName = pszSheetName;

	m_RangeExcel.AttachDispatch(m_WorksheetExcel.GetCells(),true);
}

C_XLSWorksheetWrite::~C_XLSWorksheetWrite(void)
{
}

STDMETHODIMP C_XLSWorksheetWrite::SetValue(int nRow, int nColumn, CString szValue)
{
	COleVariant vItem(szValue);
	vItem.ChangeType(VT_BSTR);
	m_RangeExcel.SetItem(COleVariant((long)(nRow)),COleVariant((long)(nColumn)),vItem);
	//m_RangeExcel.SetColumnWidth(_variant_t((long)40));
	//m_RangeExcel.AutoFit();
	//Range r;
	//r.GetEntireColumn(); 
	//r.AutoFit();
	return S_OK;
}

STDMETHODIMP C_XLSWorksheetWrite::SetValue(int nRow, int nColumn, int nValue)
{
	CString szValue;
	szValue.Format(_T("%d"), nValue);
	COleVariant vItem(szValue);

	vItem.ChangeType(VT_R8);
	m_RangeExcel.SetItem(COleVariant((long)(nRow)),COleVariant((long)(nColumn)),vItem);
	return S_OK;
}


STDMETHODIMP C_XLSWorksheetWrite::GetName(CString& szName)
{
	szName = m_szSheetName;
	return S_OK;
}

void C_XLSWorksheetWrite::Save()
{
	m_RangeExcel.ReleaseDispatch();
	m_RangeExcel.Clear();
}
