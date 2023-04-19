#pragma once

#include "PluginClient\T_PluginTemplate.h"
#include "XLSPackBundle\I_XLSWorksheet.h"

class C_XLSWorksheet : public I_XLSWorksheet
{
public:
	C_XLSWorksheet(void);
	virtual ~C_XLSWorksheet(void);

public:
	STDMETHODIMP_(int) GetRowCount();
	STDMETHODIMP_(int) GetColumnCount();
	STDMETHODIMP GetValue(int nRow, int nColumn, CString& szValue);
	STDMETHODIMP GetName(CString& szName);

public:
	BOOL Load(LPDISPATCH pWorksheetExcel);	

public:
	COleSafeArray m_OLESafeArray;

private:
	CString m_szSheetName;
};
