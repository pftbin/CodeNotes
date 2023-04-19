#pragma once

//说明：用于异步访问数据

class CDbObject
{
public:
	CDbObject(void);
	~CDbObject(void);


	BOOL GetRecordset(LPCTSTR lpszSql, _Recordset **ppRs);
	BOOL UpdateRecordset(_Recordset *pRs);
	BOOL ExecuteCommand(LPCTSTR lpszSql,long &lAffectCount);
	long GetDatabaseType(long& nType);

	HANDLE m_hWaiteHandle;
	int    m_nCmdType;
	_Recordset *m_pRs;
	LPCTSTR m_lpszSql;
	long m_lAffectCount;

	BOOL m_bUse;

	BOOL m_bRetValue;

protected:
	static void ExcuteThreadDB(LPVOID lpVoid);

	static void GetFailInfo();
	
	static DWORD m_dwLastErrorTime;
};
