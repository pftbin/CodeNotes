#include "StdAfx.h"
#include "MySQlite3.h"

MySQlite3::MySQlite3(void)
{
	m_pMyData = NULL;
	m_strTableName = _T("myTable");
}

MySQlite3::~MySQlite3(void)
{
}

BOOL MySQlite3::InitDataBase(CString strDataBasePath)
{
	if ( m_pMyData || !(m_pMyData = new CppSQLite3DB))
	{
		return FALSE;
	}

	try
	{
		USES_CONVERSION;
		char* pFileName = T2A(strDataBasePath); 
		m_pMyData->open( pFileName);
		CreateTable(_T("myTable"),_T("name,password"));
	}
	catch (CppSQLite3Exception& e)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL MySQlite3::CloseDataBase()
{
	if(m_pMyData)
	{
		m_pMyData->close();
		delete m_pMyData;
		m_pMyData = NULL;
	}
	return TRUE;
}


BOOL MySQlite3::CreateTable(CString strTableName,CString strPropDesc)
{
	if ( !m_pMyData )
	{
		return FALSE;
	}

	try
	{
		m_strTableName = strTableName;
		CString strSql = _T("create table ");
		strSql += strTableName;

		CString strCMD = _T("( ");
		CString strTemp;

		int nPos = strPropDesc.Find(_T(","));
		while (nPos != -1)
		{
			strTemp = strPropDesc.Left(nPos);
			strCMD += (strTemp + _T(" varchar(32),"));
			strPropDesc = strPropDesc.Mid(nPos+1);
			nPos = strPropDesc.Find(_T(","));
		}
		strTemp = strPropDesc.Mid(nPos);
		strCMD += (strTemp + _T(" varchar(32) )"));


		strSql += strCMD;
		USES_CONVERSION;
		char* pSqlCMD = T2A(strSql); 
		m_pMyData->execDML(pSqlCMD);
	}
	catch (CppSQLite3Exception& e)
	{
		return FALSE;
	}

	return TRUE;
}
BOOL MySQlite3::InsertItem(CString strUserName,CString strPassWord)
{
	CString strSql = _T("");
	try
	{
		strSql = _T("select password from ");
		strSql += m_strTableName;
		strSql += _T(" where name = '") + strUserName + _T("'");

		USES_CONVERSION;
		char* pSqlCMD1 = T2A(strSql);

		CppSQLite3Query q = m_pMyData->execQuery(pSqlCMD1);
		if (q.fieldIsNull(0))
		{
			TRACE("insert\n");
			strSql = _T("insert into ");
			strSql += m_strTableName + _T(" (name,password)values( '");
			strSql += strUserName + _T("','") + strPassWord +_T("' )");
		}
		else
		{
			TRACE("update\n");
			strSql = _T("update ");
			strSql += m_strTableName +_T(" set password = '");
			strSql += strPassWord +_T("' ");
			strSql += _T("where name = '") + strUserName +_T("'");
		}

		char* pSqlCMD2 = T2A(strSql); 
		m_pMyData->execDML(pSqlCMD2);
	}
	catch (CppSQLite3Exception& e)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL MySQlite3::FindItem(CString strUserName,CString strPassWord)
{
	try
	{
		CString strSelect = _T("select * from ");
		strSelect += m_strTableName;
		USES_CONVERSION;
		char* pSelect = T2A(strSelect);

		CppSQLite3Query q = m_pMyData->execQuery(pSelect);
		while(!q.eof())
		{
			//			TRACE(q.getStringField("name"));
			char* pName = T2A(strUserName);
			if (strcmp(pName,q.getStringField("name")) == 0)
			{
				//				TRACE(q.getStringField("password"));
				USES_CONVERSION;
				char* pPassWord = T2A(strPassWord);
				if (strcmp(pPassWord,q.getStringField("password")) == 0)
				{
					return TRUE;
				}
			}
			q.nextRow();
		}
		q.finalize();
	}
	catch(CppSQLite3Exception& e)
	{
		TRACE(e.errorMessage());
	}

	return FALSE;
}