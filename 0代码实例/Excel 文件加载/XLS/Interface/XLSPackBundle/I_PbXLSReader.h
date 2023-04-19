#pragma once

#include "Plugin\I_PiPlugin.h"

interface I_XLSWorksheet;

interface I_PbXLSReader : public I_PiPlugin
{
	virtual STDMETHODIMP_(BOOL) Initialize() = 0;
	virtual STDMETHODIMP_(void) Finalize() = 0;
	virtual STDMETHODIMP LoadXLS(LPCTSTR pszXLSFile, LPCTSTR pszPassword) = 0;

	virtual STDMETHODIMP_(DWORD) GetWorksheetCount() = 0;
	virtual STDMETHODIMP_(I_XLSWorksheet*) GetWorksheetAt(DWORD dwIndex) = 0;
};