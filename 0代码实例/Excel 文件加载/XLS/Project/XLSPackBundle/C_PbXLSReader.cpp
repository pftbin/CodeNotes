#include "StdAfx.h"
#include "C_PbXLSReader.h"
#include "C_XLSWorksheet.h"

C_PbXLSReader::C_PbXLSReader(LPCTSTR pszExtensionId, LPCTSTR pszPluginId)
	: T_PluginTemplate<I_PbXLSReader>(pszExtensionId, pszPluginId)
{
}

C_PbXLSReader::~C_PbXLSReader(void)
{
	Clear();
}

STDMETHODIMP C_PbXLSReader::LoadXLS(LPCTSTR pszXLSFile, LPCTSTR pszPassword)
{
	COleVariant covOptional(DISP_E_PARAMNOTFOUND,VT_ERROR);

	COleVariant covPassword = pszPassword;

	_Workbook WorkbookExcel = m_WorkbooksExcel.Open(pszXLSFile,
		covOptional, covOptional, covOptional, covPassword,
		covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional, covOptional, covOptional, 
		covOptional, covOptional);

	Sheets WorksheetsExcel = WorkbookExcel.GetWorksheets();

	long lSheetCount = WorksheetsExcel.GetCount();
	for (long lIndex = 1; lIndex <= lSheetCount; ++lIndex)
	{
		CString szMsg;
		CString szMsgName;

		C_XLSWorksheet* pWorksheet = new C_XLSWorksheet;
		_ASSERT(pWorksheet != NULL);
		if(!pWorksheet->Load(WorksheetsExcel.GetItem(COleVariant((short)lIndex))))
		{
			delete pWorksheet;
			WorkbookExcel.Close(COleVariant((short)FALSE), covOptional, covOptional);
			if (!m_WorksheetAry.empty())
				return S_OK;

			return -1;
		}

		m_WorksheetAry.push_back(pWorksheet);
	}

	WorkbookExcel.Close(COleVariant((short)FALSE), covOptional, covOptional);
	return S_OK;
}

STDMETHODIMP_(DWORD) C_PbXLSReader::GetWorksheetCount()
{
	return m_WorksheetAry.size();
}

STDMETHODIMP_(I_XLSWorksheet*) C_PbXLSReader::GetWorksheetAt(DWORD dwIndex)
{
	if (dwIndex > m_WorksheetAry.size())
		return NULL;

	return m_WorksheetAry.at(dwIndex);
}

STDMETHODIMP_(BOOL) C_PbXLSReader::Initialize()
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

	m_WorkbooksExcel = m_AppExcel.GetWorkbooks();
	m_WorksheetAry.clear();

	return TRUE;
}

STDMETHODIMP_(void) C_PbXLSReader::Finalize()
{
	m_AppExcel.Quit();
	Clear();
}

void C_PbXLSReader::Clear()
{
	WORKSHEET_VEC::iterator iterWorksheet = m_WorksheetAry.begin();
	while (iterWorksheet != m_WorksheetAry.end())
	{
		delete *iterWorksheet;
		++iterWorksheet;
	}

	m_WorksheetAry.clear();
}