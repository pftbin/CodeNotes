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
	BOOL    ExecuteStringEx(CString strSql);                       //��һ��ִ�з�ʽ
	HRESULT OpenRecordsetEx(CString strSql);                       //��һ�ַ��ʷ�ʽ

	BOOL	GetRec_Log(LPCTSTR lpszSql, _Recordset **ppRs);           //�ڶ��ַ��ʷ�ʽ
	BOOL	ExeSql_Log(LPCTSTR lpszSql,long &lAffectCount);           //�ڶ���ִ�з�ʽ
	void	GetNewGuid(CString& strId);
	int		GetSequenceNextValue(CString sSequenceName);
	CString GetWellFormTime(COleDateTime Time);

	void	RecordStatus(CString strInfo, int nType = 0);          //��־��¼
	void	RecordLogInfo(LOGINFO loginfo);

	int		GetDBServerType();  

private:

	CWriteMsgToFile			m_RcdStatus;
};
