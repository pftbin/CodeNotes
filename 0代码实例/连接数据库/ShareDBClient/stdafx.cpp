// stdafx.cpp : 只包括标准包含文件的源文件
// ShareDBClient.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "DbObject.h"

CString g_strLastError = _T("");

class CAutoInitialize
{
public:
	CAutoInitialize()
	{
		::CoInitialize(NULL);
	}
	~CAutoInitialize()
	{
		::CoUninitialize();
	}
};
CString GetNDBCLastError()
{
	g_strLastError.LockBuffer();
	CString strRet = g_strLastError;
	g_strLastError.UnlockBuffer();
	return strRet;
}

BOOL GetRecordset(LPCTSTR lpszSql, _Recordset **ppRs,CString &strError)
{
	HRESULT	hr;
	CComBSTR bstrSql;
	CAutoInitialize autoInit;
	CComPtr<IStdioDB>	pIStdioDB;
	try
	{
		hr = S_FALSE;
		for(int i = 0;i < 3&&hr != S_OK;i++,Sleep(10))
		{
			hr = pIStdioDB.CoCreateInstance(__uuidof(StdioDB), NULL, CLSCTX_ALL);
			if(hr == 0X80010005)
			{
				strError.Format(_T("GetRecordset CoCreateInstance hr=[%x][%d]"), hr, i);
				return	FALSE;
			}
		}
		if (FAILED(hr))
		{
			strError.Format(_T("GetRecordset CoCreateInstance hr=[%x]"), hr);
			return	FALSE;
		}
		bstrSql = lpszSql;
		hr	=	pIStdioDB->GetRecordset(bstrSql,ppRs);
		if( FAILED(hr))
		{
			strError.Format(_T("GetRecordset hr=[%x]"), hr);
			return	FALSE;
		}
		strError = _T("");
		return TRUE;
	}
	catch (_com_error &e)
	{
		strError.Format(_T("GetRecordset _com_error [%s][%s]"), e.Description(), e.ErrorMessage());
		return	FALSE;
	}
	catch(...)
	{
		strError = _T("GetRecordset catch 未知错误");
		return	FALSE;
	}
	return	FALSE;
}

BOOL UpdateRecordsetEx(_Recordset *pRs,CString &strError,bool bTrans = false)
{
	CAutoInitialize autoInit;
	HRESULT	hr;
	CComPtr<IStdioDB>	pIStdioDB;
	try
	{
		hr = S_FALSE;
		for(int i = 0;i < 3&&hr != S_OK;i++,Sleep(10))
		{
			hr = pIStdioDB.CoCreateInstance(__uuidof(StdioDB), NULL, CLSCTX_ALL);
			if(hr == 0X80010005)
			{
				strError.Format(_T("UpdateRecordsetEx CoCreateInstance hr=[%x],[i]"), hr, i);
				return	FALSE;
			}
		}
		if (FAILED(hr))
		{
			strError.Format(_T("UpdateRecordsetEx CoCreateInstance hr=[%x]"), hr);
			return	FALSE;
		}
		if(bTrans) pIStdioDB->DeviateTransaction();
		hr	=	pIStdioDB->UpdateRecordset(pRs);
		if(bTrans) pIStdioDB->ManualCommit();
		if( FAILED(hr))
		{
			strError.Format(_T("UpdateRecordsetEx hr=[%x]"), hr);
			return	FALSE;
		}
		strError = _T("");
		return TRUE;
	}
	catch (_com_error &e)
	{
		strError.Format(_T("UpdateRecordsetEx _com_error [%s][%s]"), e.Description(), e.ErrorMessage());
		return	FALSE;
	}
	catch(...)
	{
		strError = _T("UpdateRecordsetEx catch 未知错误");
		return	FALSE;
	}
	return	FALSE;
}

BOOL GetSqlServerSequence(LPCTSTR lpszTable, long& lSequenceID,CString &strError)
{
	HRESULT	hr;
	CComBSTR bstrTable;
	CAutoInitialize autoInit;
	CComPtr<IStdioDB>	pIStdioDB;
	try
	{
		hr = S_FALSE;
		for(int i = 0;i < 3&&hr != S_OK;i++,Sleep(10))
		{
			hr = pIStdioDB.CoCreateInstance(__uuidof(StdioDB), NULL, CLSCTX_ALL);
			if(hr == 0X80010005)
			{
				strError.Format(_T("GetSqlServerSequence CoCreateInstance hr=[%x], [i]"), hr, i);
				return	FALSE;
			}
		}
		if (FAILED(hr))
		{
			strError.Format(_T("GetSqlServerSequence CoCreateInstance hr=[%x]"), hr);
			return	FALSE;
		}
		bstrTable = lpszTable;
		DWORD dwS = GetTickCount();
		long  lTemp = 0;
		hr		=	pIStdioDB->GetNextID (bstrTable, &lTemp);
		lSequenceID = lTemp;

		if( FAILED(hr))
		{
			strError.Format(_T("GetSqlServerSequence hr=[%x]"), hr);
			return	FALSE;
		}
		strError = _T("");
		return TRUE;
	}
	catch (_com_error &e)
	{
		strError.Format(_T("GetSqlServerSequence _com_error [%s][%s]"), e.Description(), e.ErrorMessage());
		return	FALSE;
	}
	catch(...)
	{
		strError = _T("ExecuteCommand catch 未知错误");
		return	FALSE;
	}
	return	FALSE;
}

BOOL ExecuteCommand(LPCTSTR lpszSql,long &lAffectCount,CString &strError)
{
	HRESULT	hr;
	CComBSTR bstrSql;
	unsigned long ulAffectCount = 0;
	CAutoInitialize autoInit;
	CComPtr<IStdioDB>	pIStdioDB;
	try
	{
		hr = S_FALSE;
		lAffectCount = 0;
		for(int i = 0;i < 3&&hr != S_OK;i++,Sleep(10))
		{
			hr = pIStdioDB.CoCreateInstance(__uuidof(StdioDB), NULL, CLSCTX_ALL);
			if(hr == 0X80010005)
			{
				strError.Format(_T("ExecuteCommand CoCreateInstance hr=[%x], [i]"), hr, i);
				return	FALSE;
			}
		}
		if (FAILED(hr))
		{
			strError.Format(_T("ExecuteCommand CoCreateInstance hr=[%x]"), hr);
			return	FALSE;
		}
		bstrSql = lpszSql;
		DWORD dwS = GetTickCount();
		hr	=	pIStdioDB->ExecuteCommand(bstrSql,&ulAffectCount);

		if( FAILED(hr))
		{
			strError.Format(_T("ExecuteCommand ExecuteCommand hr=[%x]"), hr);
			return	FALSE;
		}
		lAffectCount = ulAffectCount;
		strError = _T("");
		return TRUE;
	}
	catch (_com_error &e)
	{
		strError.Format(_T("ExecuteCommand _com_error [%s][%s]"), e.Description(), e.ErrorMessage());
		return	FALSE;
	}
	catch(...)
	{
		strError = _T("ExecuteCommand catch 未知错误");
		return	FALSE;
	}
	return	FALSE;
}

long g_lDatabaseType = -1;
long GetDatabaseType()
{
	if (g_lDatabaseType > -1)
	{
		return g_lDatabaseType;
	}

	HRESULT	hr;
	long lDatabaseType = -1;
	CAutoInitialize autoInit;
	CComPtr<IStdioDB>	pIStdioDB;
	try
	{
		hr = S_FALSE;
		for(int i = 0;i < 3&&hr != S_OK;i++,Sleep(10))
		{
			hr = pIStdioDB.CoCreateInstance(__uuidof(StdioDB), NULL, CLSCTX_ALL);
			if(hr == 0X80010005)
			{
				AfxMessageBox(_T("错误号：0X80010005；错误说明：在消息筛选器里时，不可对外调用！"));
				return	-1;
			}
		}
		if (FAILED(hr))
		{
			ASSERT(NULL == _T("创建IDBInterface对象失败！"));
			return	-1;
		}

		hr	=	pIStdioDB->GetDatabaseType(&lDatabaseType);
		if( FAILED(hr))
		{
			return	-1;
		}

		g_lDatabaseType = lDatabaseType;
		return lDatabaseType;
	}
	catch (_com_error &e)
	{
		return	-1;
	}
	catch(...)
	{
		return	-1;
	}
	return	lDatabaseType;
}