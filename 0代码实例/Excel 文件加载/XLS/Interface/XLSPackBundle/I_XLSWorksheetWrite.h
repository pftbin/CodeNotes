#pragma once

interface I_XLSWorksheetWrite
{
	virtual STDMETHODIMP SetValue(int nRow, int nColumn, CString szValue) = 0;
	virtual STDMETHODIMP SetValue(int nRow, int nColumn, int nValue) = 0;
	virtual STDMETHODIMP GetName(CString& szName) = 0;
};