#pragma once

interface I_XLSWorksheet
{
	virtual STDMETHODIMP_(int) GetRowCount() = 0;
	virtual STDMETHODIMP_(int) GetColumnCount() = 0;
	virtual STDMETHODIMP GetValue(int nRow, int nColumn, CString& szValue) = 0;
	virtual STDMETHODIMP GetName(CString& szName) = 0;
};