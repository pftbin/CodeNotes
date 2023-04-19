#pragma once

#include "PluginClient\T_PluginTemplate.h"
#include "XLSPackBundle\I_XLSWorksheetWrite.h"

class C_XLSWorksheetWrite : public I_XLSWorksheetWrite
{
public:
	C_XLSWorksheetWrite(LPDISPATCH pWorksheetExcel, LPCTSTR pszSheetName);
	virtual ~C_XLSWorksheetWrite(void);

public:
	STDMETHODIMP SetValue(int nRow, int nColumn, CString szValue);
	STDMETHODIMP SetValue(int nRow, int nColumn, int nValue);
	STDMETHODIMP GetName(CString& szName);

public:
	void Save();

protected:
	Range m_RangeExcel;
	_Worksheet m_WorksheetExcel; 

private:
	CString m_szSheetName;
};
