#pragma once

#include <comsvcs.h>

class CCheckDbConnect
{
public:
	CCheckDbConnect(void);
	~CCheckDbConnect(void);

private:
	long m_lWriteLogFlag;
	long m_lDatabaseType;//数据库类型
	long m_lWriteEfficiencyLog;
	_ConnectionPtr m_spAdoCon;
	long m_lDBRunMode;
	BOOL m_bBeginTrans;

public:

	BOOL GetRelationSql(BSTR bstrSql,LPTSTR lpszBuf);
	HRESULT ReprotError(LPWSTR lpswError, LPWSTR lpswErrorMothod, LPCSTR lpszFileName, long lLineNo, LPWSTR lpswRemark = NULL);
	STDMETHOD(ConnectDatabase)(BOOL bIsBackupDatabase = FALSE);
	STDMETHOD(ReConnect)(_com_error *pError);
	BOOL RegGetString(LPCTSTR lpszValueName,LPCTSTR lpszDefault,LPTSTR lpszValue, BOOL bIsBackupDatabase = FALSE, BOOL bAutoCreate = FALSE);
	void RegChangeString();
	void RegWriteString(LPCTSTR lpszValueName, LPCTSTR lpszValue, BOOL bIsBackupDatabase = FALSE);
	HRESULT ReprotError(_com_error *pError, LPWSTR lpswErrorMothod, LPCSTR lpszFileName, long lLineNo, LPWSTR lpswRemark = NULL);
	HRESULT PrepareConnectPtr();

	BOOL CheckStatus();

public:
	HRESULT ExecuteCommand(BSTR bstrCmd, unsigned long *pulAffectCount);
	HRESULT GetNextID(BSTR bstrTableName, long *plNextID);
//	HRESULT DeviateTransaction();
//	HRESULT ManualCommit();
	HRESULT GetDatabaseType(long *plDatabaseType);
};
