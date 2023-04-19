#pragma once

#include "PluginClient\T_PluginTemplate.h"
#include "XLSPackBundle\I_PbXLSWriter.h"

class C_XLSWorksheetWrite;

class C_PbXLSWriter : public T_PluginTemplate<I_PbXLSWriter>
{
	typedef map<tstring, C_XLSWorksheetWrite*> WORKSHEET_MAP;

public:
	C_PbXLSWriter(LPCTSTR pszExtensionId, LPCTSTR pszPluginId);
	virtual ~C_PbXLSWriter(void);

public:
	STDMETHODIMP_(BOOL) Initialize();
	STDMETHODIMP_(void) Finalize();

	STDMETHODIMP CreateXLSSheet(LPCTSTR pszSheetName);
	STDMETHODIMP Save(LPCTSTR pszXLSFile);

	STDMETHODIMP_(DWORD) GetWorksheetCount();
	STDMETHODIMP_(I_XLSWorksheetWrite*) GetWorksheetAt(DWORD dwIndex);
	STDMETHODIMP_(I_XLSWorksheetWrite*) GetWorksheetByName(LPCTSTR pszName);

private:
	void Clear();

private:
	WORKSHEET_MAP m_WorksheetMap;
   _Application m_AppExcel;
   Workbooks m_WorkbooksExcel;
   _Workbook m_WorkbookExcel;
};

