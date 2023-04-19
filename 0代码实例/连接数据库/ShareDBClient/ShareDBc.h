#pragma once
#include "stdafx.h"
#include "isharedbc.h"
#include "etdbrecordset.h"



class CShareDBc :
	public IShareDBc
{
public:
	CShareDBc(void);
	~CShareDBc(void);

	virtual void Release(){delete this;}
	virtual BOOL IsValidShareDB();
	virtual BOOL GetCGPlayListInfo(int nID, CString& strPlayListInfo);

private:
	BOOL    ExecuteStringEx(CString strSql);                       //第一种执行方式
	HRESULT OpenRecordsetEx(CString strSql);                       //第一种访问方式

	BOOL	GetRec_Log(LPCTSTR lpszSql, _Recordset **ppRs);           //第二种访问方式
	BOOL	ExeSql_Log(LPCTSTR lpszSql,long &lAffectCount);           //第二种执行方式
	void	GetNewGuid(CString& strId);
	int		GetSequenceNextValue(CString sSequenceName);
	CString GetWellFormTime(COleDateTime Time);

	void	RecordStatus(CString strInfo, int nType = 0);          //日志记录
	void	RecordLogInfo(LOGINFO loginfo);

	int		GetDBServerType();  

private:

	CWriteMsgToFile			m_RcdStatus;
};
