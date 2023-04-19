#pragma once
#include "CppSqLite3.h"


class MySQlite3
{
public:
	MySQlite3(void);
	~MySQlite3(void);
private:
	CppSQLite3DB* m_pMyData;
	CString m_strTableName;
public:
	BOOL InitDataBase(CString strDataBasePath);
	BOOL CloseDataBase();
	BOOL CreateTable(CString strTableName,CString strPropDesc);
	BOOL InsertItem(CString strUserName,CString strPassWord);
	BOOL FindItem(CString strUserName,CString strPassWord);
};
