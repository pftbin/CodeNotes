#include "StdAfx.h"
#include "C_PbXLSWriter.h"
#include "C_XLSWorksheetWrite.h"

C_PbXLSWriter::C_PbXLSWriter(LPCTSTR pszExtensionId, LPCTSTR pszPluginId)
	: T_PluginTemplate<I_PbXLSWriter>(pszExtensionId, pszPluginId)
{
}

C_PbXLSWriter::~C_PbXLSWriter(void)
{
	Clear();
}

STDMETHODIMP C_PbXLSWriter::CreateXLSSheet(LPCTSTR pszName)
{
	COleVariant covOptional(DISP_E_PARAMNOTFOUND,VT_ERROR);

	m_WorkbookExcel = m_WorkbooksExcel.Add( covOptional );
	Sheets WorksheetsExcel = m_WorkbookExcel.GetWorksheets();

	long lSheetCount = WorksheetsExcel.GetCount();
	_Worksheet sheet = NULL;
	for (long lIndex = 1; lIndex <= lSheetCount; ++lIndex)
	{
		sheet = WorksheetsExcel.GetItem(COleVariant((short)lIndex));

		WORKSHEET_MAP::iterator ite = m_WorksheetMap.find(sheet.GetName().GetBuffer());
		if (ite != m_WorksheetMap.end())
		{
			m_WorksheetMap.erase(ite);
			delete (C_XLSWorksheetWrite*)(ite->second);

			sheet.Delete();
			sheet = WorksheetsExcel.Add(covOptional,covOptional, covOptional, covOptional);
			break;
		}

		sheet = NULL;
	}

	C_XLSWorksheetWrite* pWorksheet = NULL;
	if (!sheet)
		pWorksheet = new(nothrow) C_XLSWorksheetWrite(WorksheetsExcel.GetItem(COleVariant((short)1)), pszName);
	else
		pWorksheet = new(nothrow) C_XLSWorksheetWrite(sheet, pszName);
	
	m_WorksheetMap.insert(WORKSHEET_MAP::value_type(pszName, pWorksheet));


	return S_OK;
}

STDMETHODIMP C_PbXLSWriter::Save(LPCTSTR pszXLSFile)
{

	WORKSHEET_MAP::iterator iterWorksheet = m_WorksheetMap.begin();
	while (iterWorksheet != m_WorksheetMap.end())
	{
		C_XLSWorksheetWrite* pWorksheet = iterWorksheet->second;
		pWorksheet->Save();
		++iterWorksheet;
	}

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	try
	{
		m_WorkbookExcel.SaveAs(COleVariant(pszXLSFile),covOptional,covOptional,covOptional,
			covOptional,covOptional,(long)0,covOptional,covOptional,covOptional,covOptional,covOptional);
	}
	catch (...)
	{
		return S_FALSE;
	}
	
	m_WorkbookExcel.Close(COleVariant((short)FALSE),covOptional,covOptional);
	m_WorkbooksExcel.Close();

	return S_OK;
}

STDMETHODIMP_(BOOL) C_PbXLSWriter::Initialize()
{
	if (m_AppExcel.m_lpDispatch == NULL) 
	{
		BOOL bRet = FALSE;
		if (!m_AppExcel.CreateDispatch(_T("Excel.Application")))
		{
			m_AppExcel.m_lpDispatch = NULL;
			return FALSE;
		}
	}

	m_AppExcel.SetAlertBeforeOverwriting(FALSE);
	m_AppExcel.SetDisplayAlerts(FALSE);

	m_WorkbooksExcel = m_AppExcel.GetWorkbooks();

	Clear();
	return TRUE;
}

STDMETHODIMP_(void) C_PbXLSWriter::Finalize()
{
	m_AppExcel.Quit();
	Clear();
}

STDMETHODIMP_(DWORD) C_PbXLSWriter::GetWorksheetCount()
{
	return m_WorksheetMap.size();
}

STDMETHODIMP_(I_XLSWorksheetWrite*) C_PbXLSWriter::GetWorksheetAt(DWORD dwIndex)
{
	if (dwIndex >= m_WorksheetMap.size())
		return NULL;

	WORKSHEET_MAP::iterator ite = m_WorksheetMap.begin();
	WORKSHEET_MAP::iterator iteEnd = m_WorksheetMap.end();
	for (DWORD i=0; i<dwIndex; ++i)
		++ite;

	if (ite == iteEnd)
		return NULL;

	return ite->second;
}

STDMETHODIMP_(I_XLSWorksheetWrite*) C_PbXLSWriter::GetWorksheetByName(LPCTSTR pszName)
{
	if (EMPTY_STR(pszName))
		return NULL;

	WORKSHEET_MAP::iterator ite = m_WorksheetMap.find(pszName);
	if (ite == m_WorksheetMap.end())
		return NULL;

	return ite->second;
}


void C_PbXLSWriter::Clear()
{
	WORKSHEET_MAP::iterator ite = m_WorksheetMap.begin();
	while (ite != m_WorksheetMap.end())
	{
		delete ite->second;
		++ite;
	}

	m_WorksheetMap.clear();
}
