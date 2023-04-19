#include "StdAfx.h"
#include "ShareDBc.h"

#ifdef XP
#import ".\SmmQueryInterfaceBL.dll" no_namespace
#endif

extern BOOL GetRecordset(LPCTSTR lpszSql, _Recordset **ppRs,CString &strError);
extern BOOL UpdateRecordsetEx(_Recordset *pRs,CString &strError,bool bTrans = false);
extern BOOL ExecuteCommand(LPCTSTR lpszSql,long &lAffectCount,CString &strError);
extern BOOL GetSqlServerSequence(LPCTSTR lpszTable, long& lSequenceID,CString &strError);
extern long GetDatabaseType();
extern CString GetNDBCLastError();

CShareDBc::CShareDBc(void)
{
	CoInitialize(NULL);
}

CShareDBc::~CShareDBc(void)
{
	CoUninitialize();
}

HRESULT CShareDBc::OpenRecordsetEx(CString strSql)
{
	BSTR bstrSql1 = strSql.AllocSysString();
	CETDBRecordset rs;
	CString strError;
	CString strInfo;
	if (!GetRecordset(strSql,&rs,strError))
	{
		strInfo.Format(_T("执行语句失败,%s, hr=%s"),strSql,strError);
		RecordStatus(strInfo);
		return S_FALSE;
	}
	else
	{
		strInfo.Format(_T("执行语句成功,%s"),strSql);
		RecordStatus(strInfo);

	}
	return S_OK;
}

BOOL CShareDBc::ExecuteStringEx(CString strSql)
{
	long lAffectCount=0;
	try
	{
		BSTR bstrSql1 = strSql.AllocSysString();
		CString strError;		
		CString strInfo;
		if (!ExecuteCommand(bstrSql1,lAffectCount,strError))
		{
			strInfo.Format(_T("执行语句失败 ,%s, hr=%s"),strSql,strError);
			RecordStatus(strInfo);
		}
		else
		{
			strInfo.Format(_T("执行语句成功,%s"),strSql);
			RecordStatus(strInfo);
			return TRUE;
		}		
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}

IMPLEMENT_LOGER(LogerDbc)
void CShareDBc::RecordLogInfo(LOGINFO loginfo)
{
	CString strInfo;
	strInfo.Format(_T("%s"), loginfo.strInfo);
	loginfo.strInfo=strInfo;
	LogerDbc.WriteLogInfo(loginfo);
}

BOOL CShareDBc::GetRec_Log(LPCTSTR lpszSql, _Recordset **ppRs)
{
	DWORD dwS = GetTickCount();
	CString strError;
	BOOL b = GetRecordset(lpszSql, ppRs,strError);
	DWORD dwE = GetTickCount();
	if (dwE - dwS > 200 || b == FALSE)
	{
		CString s;
		if (b)
			s.Format(_T("GetRec_Log 执行成功 耗时%d, %s"), dwE - dwS, lpszSql);
		else
			s.Format(_T("GetRec_Log 执行失败 耗时%d[%s], %s"), dwE - dwS, strError, lpszSql);
		RecordStatus(s);
	}
	return b;
}

BOOL CShareDBc::ExeSql_Log(LPCTSTR lpszSql,long &lAffectCount)
{
	::CoInitialize(NULL);
	DWORD dwS = GetTickCount();
	CString strError;
	BOOL b = ExecuteCommand(lpszSql, lAffectCount,strError);
	DWORD dwE = GetTickCount();
	if (dwE - dwS > 100 || b == FALSE)
	{
		CString s;
		if (b)
			s.Format(_T("ExeSql_Log 执行成功 耗时%d, %s"), dwE - dwS, lpszSql);
		else
			s.Format(_T("ExeSql_Log 执行失败 耗时%d[%s], %s"), dwE - dwS,strError, lpszSql);
		RecordStatus(s);
	}
	return b;
}

int CShareDBc::GetDBServerType()
{
	long ntype=0;
	ntype = GetDatabaseType();
	return ntype;
}

void CShareDBc::GetNewGuid(CString& strId)
{
	UUID uuid;
	unsigned short* p=NULL;
	UuidCreate(&uuid);
	UuidToString(&uuid,&p);
	TCHAR* pG = (TCHAR*)p;
	strId = pG;
	strId.Replace(_T("'"), _T("''"));
}

int CShareDBc::GetSequenceNextValue(CString sSequenceName)
{
	int nNexVal = 0;
	try
	{
		CETDBRecordset rs;
		CString sTemp,sSql;
		switch(GetDatabaseType())
		{
		case DB_ORACLE:
			sTemp = L"select %s.NEXTVAL FROM DUAL";
			sSql.Format(sTemp,sSequenceName);
			if( !GetRec_Log(sSql, &rs))
				return FALSE;
			nNexVal = rs.FieldInt(_T("NEXTVAL"));
			break;
		case DB_DB2:
			sTemp = L"select nextval for %s as NEXTVAL from sysibm.sysdummy1";
			sSql.Format(sTemp,sSequenceName);
			if( !GetRec_Log(sSql, &rs))
				return FALSE;
			nNexVal = rs.FieldInt(_T("nextval"));
			break;
		case DB_SQLSERVER:
			long lSequenceID = 0;
			CString strError;
			if(!GetSqlServerSequence(sSequenceName, lSequenceID, strError))
			{
				RecordStatus(strError);
			}
			nNexVal = lSequenceID;
			break;
		}
	}
	catch (_com_error e)
	{
		RECORD_ERROR(L"GetSequenceNextValue",e);
		if (nNexVal == 0)
			nNexVal = GetTickCount();
		return nNexVal;
	}
	if (nNexVal == 0)
		nNexVal = GetTickCount();
	return nNexVal;	
}

CString CShareDBc::GetWellFormTime(COleDateTime Time)
{
	CString strTime = Time.Format(L"%Y-%m-%d %H:%M:%S");
	CString bstrSec = L"";

	int iDataType = GetDatabaseType();

	if (iDataType != DB_ORACLE && 
		iDataType != DB_DB2 && 
		iDataType != DB_SQLSERVER)
	{
		iDataType = GetDatabaseType();
	}

	switch(iDataType) {
	case DB_ORACLE:
		bstrSec = _T("TO_DATE('");
		bstrSec += CString(strTime);
		bstrSec += _T("','YYYY-MM-DD HH24:MI:SS')");
		break;
	case DB_DB2:
	case DB_SQLSERVER:
		bstrSec = _T("'");
		bstrSec += CString(strTime);
		bstrSec += _T("'");
		break;

	default:
		break;	
	}

	if (bstrSec.IsEmpty ())
	{
		CString strInfo;
		strInfo.Format(_T("获取数据库类型出错,获取类型：%d"),iDataType);
		bstrSec = _T("");
		RecordStatus(strInfo);
	}
	return bstrSec;
}

void CShareDBc::RecordStatus(CString strInfo, int nType)
{
	//m_RcdStatus.WriteLog(strInfo, nType);
}

//////////////////////////////////////////////////////////////////////////
BOOL CShareDBc::IsValidShareDB()
{
	long lDBType = GetDatabaseType();
	return (lDBType!=-1)?TRUE:FALSE;
}

BOOL CShareDBc::GetCGPlayListInfo(int nID, CString& strPlayListInfo)
{
	try
	{
		CETDBRecordset rs;
		CString strSql;
		strSql.Format(_T("SELECT STRXML FROM SBT_CGPLAYLIST WHERE NID = %d"), nID);
		if(!GetRec_Log(strSql, &rs))
		{
			return FALSE;
		}
		if(rs.IsEOF())
		{
			return FALSE;
		}
		else
		{
			strPlayListInfo	= rs.FieldString(_T("STRXML"));
		}
	}
	catch(_com_error &e)
	{
		RECORD_ERROR(L"GetCGPlayListInfo",e);
		strPlayListInfo.Empty();

		return FALSE;
	}

	return TRUE;
}