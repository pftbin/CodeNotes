#pragma once

#include "PluginClient\T_PluginTemplate.h"
#include "XLSPackBundle\I_PbXLSReader.h"

class C_XLSWorksheet;

class C_PbXLSReader : public T_PluginTemplate<I_PbXLSReader>
{
	typedef vector<C_XLSWorksheet*> WORKSHEET_VEC;

public:
	C_PbXLSReader(LPCTSTR pszExtensionId, LPCTSTR pszPluginId);
	virtual ~C_PbXLSReader(void);

public:
	STDMETHODIMP_(BOOL) Initialize();
	STDMETHODIMP_(void) Finalize();

	STDMETHODIMP LoadXLS(LPCTSTR pszXLSFile, LPCTSTR pszPassword);

	STDMETHODIMP_(DWORD) GetWorksheetCount();
	STDMETHODIMP_(I_XLSWorksheet*) GetWorksheetAt(DWORD dwIndex);

private:
	void Clear();

private:
   WORKSHEET_VEC m_WorksheetAry;

   _Application m_AppExcel;
   Workbooks m_WorkbooksExcel;
};

