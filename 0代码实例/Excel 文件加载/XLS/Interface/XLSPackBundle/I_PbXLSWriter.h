#pragma once

#include "Plugin\I_PiPlugin.h"

interface I_XLSWorksheetWrite;

interface I_PbXLSWriter : public I_PiPlugin
{
	virtual STDMETHODIMP_(BOOL) Initialize() = 0;
	virtual STDMETHODIMP_(void) Finalize() = 0;
	virtual STDMETHODIMP CreateXLSSheet(LPCTSTR pszSheeetName) = 0;
	virtual STDMETHODIMP Save(LPCTSTR pszXLSFile) = 0;

	virtual STDMETHODIMP_(DWORD) GetWorksheetCount() = 0;
	virtual STDMETHODIMP_(I_XLSWorksheetWrite*) GetWorksheetAt(DWORD dwIndex) = 0;
	virtual STDMETHODIMP_(I_XLSWorksheetWrite*) GetWorksheetByName(LPCTSTR pszName) = 0;
};