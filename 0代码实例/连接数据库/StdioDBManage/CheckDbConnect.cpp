#include "stdafx.h"
#include "CheckDbConnect.h"
#include "StdioDBManage.h"
#include "oledb.h"

#include "Mmsystem.h"
#pragma comment ( lib, "Winmm.lib" )

#define  WHERE_LENGTH	15

CCheckDbConnect::CCheckDbConnect(void)
{
	m_lDatabaseType = DB_ORACLE;
	m_spAdoCon = NULL;
	m_lDBRunMode = 0;
	m_lWriteLogFlag = 0;
	m_lWriteEfficiencyLog = 0;
	m_bBeginTrans = FALSE;

	
}

CCheckDbConnect::~CCheckDbConnect(void)
{
	long lState;
	if(m_spAdoCon)
	{
		m_spAdoCon->get_State(&lState);
		if(lState != adStateClosed)
		{
			m_spAdoCon->Close();
		}
		m_spAdoCon.Release();
	}
}






HRESULT CCheckDbConnect::GetDatabaseType(long *plDatabaseType)
{
	*plDatabaseType = m_lDatabaseType;
	return S_OK;
}

HRESULT CCheckDbConnect::ReConnect(_com_error *pError)
{
	int i;
	HRESULT hr;
	long lState;
	wchar_t pwstrTemp[21];
	wchar_t *pwstrError;
	_bstr_t bstrError;
	BOOL bIsBackupDatebase,bNeedReConnect = FALSE;
	try
	{
		if(m_spAdoCon)
		{
			m_spAdoCon->get_State(&lState);
			if(lState == adStateClosed)
			{
				m_spAdoCon.Release();
				bNeedReConnect = TRUE;
			}
		}
		else
		{
			bNeedReConnect = TRUE;
		}

		// 如果其他线程已经切到备库了，则自己也切到备库
		bIsBackupDatebase = FALSE;
//		if (g_bIsUserBackupDatebase == TRUE && m_bIsBackupDatabase == FALSE)
//		{
//			bNeedReConnect = TRUE;
//			bIsBackupDatebase = TRUE;
//		}

		if(bNeedReConnect)
			goto ReConnectDB;

		hr = pError->Error();
		if(m_lDatabaseType == DB_ORACLE)
		{
			if(hr == E_POINTER)//无效指针，表示数据库连接无效
			{
				m_spAdoCon = NULL;
				goto ReConnectDB;
			}
			//连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。所以需要重新连接。
			//对象关闭时，不允许操作。所以需要重新连接。
			else if(hr == 0X800A0E7D||hr == 0X800A0E78)
			{
				goto ReConnectDB;
			}

			bstrError = pError->Description();
			pwstrError = (wchar_t *)bstrError;
			for(i = 0;i < 10&&pwstrError[i] != L':'&&pwstrError[i] != L'：';i++)
			{
				pwstrTemp[i] = pwstrError[i];
			}
			pwstrTemp[9] = 0;
			if(wcscmp(pwstrTemp,L"ORA-03114") == 0)//Oracle连接已断开
			{
				goto ReConnectDB;
			}
			else if(wcscmp(pwstrTemp,L"ORA-03113") == 0)//一种常见的Oracle连接断开的错误
			{
				goto ReConnectDB;
			}
			else if(wcscmp(pwstrTemp,L"ORA-01012") == 0)//一种Oracle没有登录的错误。
			{
				goto ReConnectDB;
			}
			else if(wcscmp(pwstrTemp,L"ORA-01014") == 0)//DB_ORACLE shutdown in progress
			{
				goto ReConnectDB;
			}
			else if(wcscmp(pwstrTemp,L"ORA-00028") == 0)//your session has been killed
			{
				goto ReConnectDB;
			}
			else if(wcscmp(pwstrTemp,L"ORA-01034") == 0)//DB_ORACLE not available
			{
				goto ReConnectDB;
			}
			else if(wcscmp(pwstrTemp,L"ORA-01033") == 0)//DB_ORACLE initialization or shutdown in progress
			{
				goto ReConnectDB;
			}
		}
		else if(m_lDatabaseType == DB_SQLSERVER)
		{
			if(hr == 0x800a0e7d || hr == 0x80004005) // 数据库或网络连接重启
				goto ReConnectDB;
		}
		else if (m_lDatabaseType == DB_DB2)
		{
			//执行一次读取当前时间的SQL语句来检测数据库是否可用
			_bstr_t bstrSql;
			_variant_t var;
			try
			{
				bstrSql = L"SELECT CURRENT TIMESTAMP FROM SYSIBM.SYSDUMMY1";
				m_spAdoCon->Execute(bstrSql, &var, adCmdText | adExecuteNoRecords);
			}
			catch(_com_error e)
			{
				goto ReConnectDB;
			}
		}
		else if(m_lDatabaseType == DB_MYSQL)
		{
			//执行一次读取当前时间的SQL语句来检测数据库是否可用
			_bstr_t bstrSql;
			_variant_t var;
			try
			{
				bstrSql = L"select now()";
				m_spAdoCon->Execute(bstrSql, &var, adCmdText | adExecuteNoRecords);
			}
			catch(_com_error e)
			{
				goto ReConnectDB;
			}
		}

		return E_FAIL;
	}
	catch(_com_error e)
	{
		ReprotError(&e, L"CCheckDbConnect::ReConnect", __FILE__,__LINE__);
		return E_FAIL;
	}
	catch(...)
	{
		ReprotError(_T("出现普通异常"), L"CCheckDbConnect::ReConnect",__FILE__,__LINE__);
		return E_FAIL;
	}

ReConnectDB:
	try
	{
		if (m_spAdoCon)
		{
			m_spAdoCon->get_State(&lState);
			if(lState != adStateClosed)
			{
				m_spAdoCon->Close();
			}
			m_spAdoCon.Release();
		}	

		// 如果连接超过时限也不成功，则自动切换到备用数据库
		if (bIsBackupDatebase)
		{
			hr = ConnectDatabase(TRUE); // 如果有其他线程已经切到备库了，则直接连接备库，如果连接3次都失败，则返回E_FAIL
			if (SUCCEEDED(hr))
			{
				// 修改主库连接信息为备库
				RegChangeString();
//				g_bIsUserBackupDatebase = TRUE;
				ReprotError(L"切换到备用数据库成功！", L"CCheckDbConnect::ReConnect", __FILE__,__LINE__, NULL);
			}
		}
		else
		{
			hr = ConnectDatabase();
			if (CheckStatus())
			{
//				g_dwFaildTime = 0;
			}
			else
			{
				DWORD dwCurrentTime = timeGetTime();
				TCHAR strDelayTiem[20];
				RegGetString(_T("DelayTime"), _T("0"), strDelayTiem,FALSE,TRUE);
				long lDelayTime = _ttol(strDelayTiem);
//				if (lDelayTime != 0 && g_dwFaildTime != 0 && abs(dwCurrentTime - g_dwFaildTime) > lDelayTime*1000) // 连接中断超过设定时间，则自动连接备用数据库
				{
					hr = ConnectDatabase(TRUE);
					if (SUCCEEDED(hr))
					{
						// 修改主库连接信息为备库
						RegChangeString();
//					g_bIsUserBackupDatebase = TRUE;
						ReprotError(L"切换到备用数据库成功！", L"CCheckDbConnect::ReConnect", __FILE__,__LINE__, NULL);
					}
				}
			}
		}
		return hr;
	}
	catch(_com_error e)
	{
		ReprotError(&e, L"CCheckDbConnect::ReConnect", __FILE__,__LINE__);
		return E_FAIL;
	}
	catch(...)
	{
		ReprotError(_T("出现普通异常"), L"CCheckDbConnect::ReConnect",__FILE__,__LINE__);
		return E_FAIL;
	}
	return E_FAIL;
}

HRESULT CCheckDbConnect::ConnectDatabase(BOOL bIsBackupDatabase /* = FALSE */)
{
	HRESULT hr;
	long lState;
	try
	{
//		if (g_dwFaildTime == 0)
//			g_dwFaildTime = timeGetTime();

		if(m_spAdoCon)
		{
			m_spAdoCon->get_State(&lState);
			if(lState != adStateClosed)
			{
				m_spAdoCon->Close();
			}
			m_spAdoCon.Release();
		}

		hr = m_spAdoCon.CreateInstance(__uuidof(Connection));
		if(FAILED(hr))
		{
			ReprotError(L"创建组件Connection失败！", L"CCheckDbConnect::ConnectDatabase", __FILE__,__LINE__, NULL);
			m_spAdoCon.Release();
			return E_FAIL;
		}
		if(m_spAdoCon == NULL)
		{
			ReprotError(L"创建组件Connection对象指针为空！", L"CCheckDbConnect::ConnectDatabase", __FILE__,__LINE__, NULL);
			return E_FAIL;
		}
		TCHAR strDatabaseType[100],strServerName[100],strUserID[100],strConstructString[100],strDatabaseName[100],strConnectionString[255],strDBRunMode[20],strWriteLogFlag[20];
		TCHAR strPort[100] = {0};
		RegGetString(_T("ServerName"), _T("cctvdb"), strServerName, bIsBackupDatabase,TRUE);

		memset(strWriteLogFlag,0,20*sizeof(TCHAR));
		RegGetString(_T("WriteLogFlag"), _T("0"), strWriteLogFlag, bIsBackupDatabase);
		m_lWriteLogFlag = _ttol(strWriteLogFlag);
		RegGetString(_T("DatabaseType"), _T("Oracle"), strDatabaseType, bIsBackupDatabase,TRUE);
		RegGetString(_T("UserID"), _T("cctvdba"), strUserID, bIsBackupDatabase,TRUE);
		RegGetString(_T("Password"), _T("cctvdba"), strConstructString, bIsBackupDatabase,TRUE);
		RegGetString(_T("DatabaseName"), _T(""), strDatabaseName, bIsBackupDatabase,TRUE);
		memset(strDBRunMode,0,20*sizeof(TCHAR));
		RegGetString(_T("DBRunMode"), _T("0"), strDBRunMode, bIsBackupDatabase);
		m_lDBRunMode = _ttol(strDBRunMode);
		memset(strDBRunMode,0,20*sizeof(TCHAR));
		RegGetString(_T("WriteEfficiencyLog"), _T("0"), strDBRunMode, bIsBackupDatabase);
		m_lWriteEfficiencyLog = _ttol(strDBRunMode);
		_tcsupr(strDatabaseType);
		if(_tcscmp(strDatabaseType,_T("ORACLE"))==0)
		{
			wsprintf( strConnectionString, _T("provider=oraoledb.oracle;data source=%s"),strServerName);
			m_lDatabaseType = DB_ORACLE;
		}
		else if(_tcscmp(strDatabaseType,_T("SQLSERVER"))==0)
		{
			wsprintf( strConnectionString, _T("provider=sqloledb;data source=%s;Initial Catalog=%s"),strServerName,strDatabaseName);
			m_lDatabaseType = DB_SQLSERVER;
		}
		else if(_tcscmp(strDatabaseType,_T("SYBASE"))==0)
		{
			wsprintf( strConnectionString, _T("Provider=MSDASQL.1;DRIVER={Sybase System 11};SRVR=%s;DB=%s;"),strServerName,strDatabaseName);
			m_lDatabaseType = DB_SYBASE;
		}
		else if (_tcscmp(strDatabaseType, _T("DB2")) == 0)
		{
			wsprintf(strConnectionString, 
				_T("Provider=IBMDADB2.1;Location=%s;Data Source=%s;"), 
				strServerName, 
				strDatabaseName);
			m_lDatabaseType = DB_DB2;
			Sleep(100);
		}
		else if(_tcscmp(strDatabaseType, _T("ACCESS")) == 0)
		{
			//strConn.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password= %s"),strServerName);
			wsprintf(strConnectionString, 
				_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s"), 
				strServerName);
			m_lDatabaseType = DB_ACCESS;
		}
		else if(_tcscmp(strDatabaseType, _T("MYSQL")) == 0)
		{
			RegGetString(_T("port"), _T("3306"), strPort, bIsBackupDatabase,TRUE);
			wsprintf(strConnectionString, 
				_T("Driver={MySQL ODBC 5.1 Driver};server=%s;database=%s;port=%s;OPTION=3"),strServerName,strDatabaseName,strPort);
				//_T("Driver={MySQL ODBC 5.1 Driver};server=%s;database=%s;OPTION=3"),strServerName,strDatabaseName);
			m_lDatabaseType = DB_MYSQL;
			//ReprotError(L"数据库类型：Mysql！", L"CDBInterface::ConnectDatabase", __FILE__,__LINE__, NULL);
		}
		else
		{
			m_lDatabaseType = 0;
			ReprotError(L"设置的数据库类型非法！", L"CCheckDbConnect::ConnectDatabase", __FILE__,__LINE__, NULL);
			m_spAdoCon.Release();
			return E_FAIL;
		}

		if(m_lDatabaseType == DB_ORACLE)
		{
			// 使用从注册表中读取的构造者字符串连接数据库
			//hr = m_spAdoCon->Open(strConnectionString,strUserID,m_bstrConstr,adAsyncConnect);
			hr = m_spAdoCon->Open(strConnectionString,strUserID,strConstructString,adAsyncConnect);

			if(FAILED(hr))
			{
				ReprotError(L"连接数据库失败！", L"CCheckDbConnect::ConnectDatabase", __FILE__,__LINE__, NULL);
				m_spAdoCon.Release();
				return hr;
			}

			m_spAdoCon->get_State(&lState);
			while(lState == adStateConnecting)
			{
				Sleep(5);

				m_spAdoCon->get_State(&lState);
			}
			Sleep(10);

			m_spAdoCon->get_State(&lState);
			if(lState != adStateOpen)
			{
				ReprotError(L"连接数据库的参数不正确失败！", L"CCheckDbConnect::ConnectDatabase", __FILE__,__LINE__, NULL);
				m_spAdoCon.Release();
				return E_FAIL;
			}
		}
		else
		{
			// 使用从注册表中读取的构造者字符串连接数据库
			hr = m_spAdoCon->Open(strConnectionString,strUserID,strConstructString,adConnectUnspecified);
			if(FAILED(hr))
			{
				ReprotError(L"连接数据库失败！", L"CCheckDbConnect::ConnectDatabase", __FILE__,__LINE__, NULL);
				m_spAdoCon.Release();
				return hr;
			}
		}
		ReprotError(L"连接数据库成功！", L"CCheckDbConnect::ConnectDatabase", __FILE__,__LINE__, NULL);
		//如果是已经执行了开始事务的，在重连接后也必须开启事物
		if(m_bBeginTrans)
		{
			m_spAdoCon->BeginTrans ();
		}
	}
	catch(_com_error e)
	{
		m_spAdoCon.Release();
		return ReprotError(&e, L"CCheckDbConnect::ConnectDatabase", __FILE__,__LINE__);
	}
	catch(...)
	{
		ReprotError(_T("出现普通异常"), L"CCheckDbConnect::ConnectDatabase",__FILE__,__LINE__,NULL);
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CCheckDbConnect::PrepareConnectPtr()
{
	long lState = 0;
	BOOL bNeedReConnect = FALSE;
	try
	{
		if(m_spAdoCon)
		{
			m_spAdoCon->get_State(&lState);
			if(lState == adStateClosed)
			{
				m_spAdoCon.Release();
				bNeedReConnect = TRUE;
			}
		}
		else if (m_spAdoCon == NULL)
		{
			bNeedReConnect = TRUE;
		}

		// 如果其他线程已经切到备库了，则自己也切到备库
//		BOOL bIsBackupDatebase = FALSE;
//		if (g_bIsUserBackupDatebase == TRUE && m_bIsBackupDatabase == FALSE)
		{
			bNeedReConnect = TRUE;
//			bIsBackupDatebase = TRUE;
		}

		if(!bNeedReConnect)
		{
			return S_OK;
		}
		else
		{
			ReprotError(L"需要重连接数据库", L"CCheckDbConnect::ReConnect",__FILE__,__LINE__,L"进入有效连接判断！");

			HRESULT hr;
			hr = ConnectDatabase();

			//需要执行SQL语句才能正确判断
			if (SUCCEEDED(hr) && CheckStatus())
				return S_OK;
			else
				return E_FAIL;
		}
	}
	catch(_com_error e)
	{
		return ReprotError(&e, L"CCheckDbConnect::PrepareConnectPtr", __FILE__,__LINE__);
	}
	catch(...)
	{
		ReprotError(_T("出现普通异常"), L"CCheckDbConnect::PrepareConnectPtr",__FILE__,__LINE__);
		return E_FAIL;
	}
	return E_FAIL;
}

BOOL CCheckDbConnect::CheckStatus()
{
	long lState;
	_variant_t var;
	_bstr_t bstrCmd;
	try
	{
		if(m_spAdoCon == NULL)
			return FALSE;
		m_spAdoCon->get_State(&lState);
		if(lState == adStateClosed)
			return FALSE;

		if(m_lDatabaseType == DB_ORACLE)
			bstrCmd = L"SELECT SYSDATE FROM DUAL";
		else if (m_lDatabaseType == DB_SQLSERVER || m_lDatabaseType == DB_SYBASE)
			bstrCmd = L"select getdate()";
		else if (m_lDatabaseType == DB_DB2)
			bstrCmd = L"select current timestamp from sysibm.sysdummy1";
		else if(m_lDatabaseType == DB_MYSQL)
		{
			bstrCmd = L"select now()";
		}

		m_spAdoCon->Execute(bstrCmd, &var, adCmdText | adExecuteNoRecords);
	}
	catch(_com_error e)
	{
		ReprotError(&e, L"CCheckDbConnect::CheckDBStatus", __FILE__,__LINE__);
		return FALSE;
	}
	catch(...)
	{
		ReprotError(_T("出现普通异常"), L"CCheckDbConnect::CheckStatus",__FILE__,__LINE__);
		return FALSE;
	}
	return TRUE;
}



HRESULT CCheckDbConnect::ReprotError(_com_error *pError, LPWSTR lpswErrorMothod, LPCSTR lpszFileName, long lLineNo, LPWSTR lpswRemark)
{
	DWORD dwSize;
	TCHAR __buf[40];
	TCHAR __Userbuf[255];
	TCHAR __Compbuf[255];
	TCHAR __FileNamebuf[512];
	_bstr_t bstrDescription;
	_bstr_t bstrErrorNo;
	BSTR bstrErrorDescription,bstrComputer,bstrErrorMothod,bstrFileName,bstrRemark,bstrAuthorName;
	try
	{
		dwSize = 255;
		memset(__Userbuf,0,sizeof(TCHAR)*255);
		GetUserName(__Userbuf,&dwSize);
		dwSize = 255;
		memset(__Compbuf,0,sizeof(TCHAR)*255);
		GetComputerName(__Compbuf,&dwSize);
		memset(__buf,0,sizeof(TCHAR)*40);
		wsprintf(__buf, _T("(0X%x)"), pError->Error());
		memset(__FileNamebuf,0,sizeof(TCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, lpszFileName, -1, __FileNamebuf,512);
		bstrDescription = pError->Description();
		bstrComputer =  ::SysAllocString(__Compbuf);
		bstrErrorMothod = ::SysAllocString(lpswErrorMothod);
		bstrFileName = ::SysAllocString(__FileNamebuf);
		bstrAuthorName = ::SysAllocString(__Userbuf);

		bstrErrorNo = __buf;
		bstrDescription = bstrErrorNo + bstrDescription;
		bstrErrorDescription = ::SysAllocString(bstrDescription);
		if(lpswRemark)
			bstrRemark = ::SysAllocString(lpswRemark);
		else
			bstrRemark = ::SysAllocString(L"");
		WriteLog(bstrErrorDescription, bstrComputer, 0, bstrErrorMothod, bstrFileName, lLineNo, bstrAuthorName, bstrRemark);
	}
	catch(_com_error e)
	{
	}
	catch(...)
	{
	}
	::SysFreeString(bstrErrorDescription);
	::SysFreeString(bstrComputer);
	::SysFreeString(bstrErrorMothod);
	::SysFreeString(bstrFileName);
	::SysFreeString(bstrRemark);
	::SysFreeString(bstrAuthorName);
	return E_FAIL;
}

HRESULT CCheckDbConnect::ReprotError(LPWSTR lpswError, LPWSTR lpswErrorMothod, LPCSTR lpszFileName, long lLineNo, LPWSTR lpswRemark)
{
	HRESULT hr;
	DWORD dwSize;
	TCHAR __Userbuf[255];
	TCHAR __Compbuf[255];
	TCHAR __FileNamebuf[512];
	BSTR bstrErrorDescription,bstrComputer,bstrErrorMothod,bstrFileName,bstrRemark,bstrAuthorName;

	try
	{
		dwSize = 255;
		memset(__Userbuf,0,sizeof(TCHAR)*255);
		GetUserName(__Userbuf,&dwSize);
		dwSize = 255;
		memset(__Compbuf,0,sizeof(TCHAR)*255);
		GetComputerName(__Compbuf,&dwSize);
		memset(__FileNamebuf,0,sizeof(TCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, lpszFileName, -1, __FileNamebuf,512);
		bstrErrorDescription = ::SysAllocString(lpswError);
		bstrComputer = ::SysAllocString(__Compbuf);
		bstrErrorMothod = ::SysAllocString(lpswErrorMothod);
		bstrFileName = ::SysAllocString(__FileNamebuf);
		if(lpswRemark)
			bstrRemark = ::SysAllocString(lpswRemark);
		else
			bstrRemark = ::SysAllocString(_T(""));
		bstrAuthorName = ::SysAllocString(__Userbuf);
		hr = WriteLog(bstrErrorDescription, bstrComputer, 0, bstrErrorMothod, bstrFileName, lLineNo, bstrAuthorName, bstrRemark);
	}
	catch(_com_error e)
	{
	}
	catch(...)
	{
	}
	::SysFreeString(bstrErrorDescription);
	::SysFreeString(bstrComputer);
	::SysFreeString(bstrErrorMothod);
	::SysFreeString(bstrFileName);
	::SysFreeString(bstrRemark);
	::SysFreeString(bstrAuthorName);
	return E_FAIL;
}



void CCheckDbConnect::RegChangeString()
{
	TCHAR strDatabaseType[100],strServerName[100],strUserID[100],strConstructString[100],strDatabaseName[100];
	RegGetString(_T("DatabaseType"), _T("Oracle"), strDatabaseType, TRUE);
	RegGetString(_T("ServerName"), _T("cctvdb"), strServerName, TRUE);
	RegGetString(_T("UserID"), _T("cctvdba"), strUserID, TRUE);
	RegGetString(_T("Password"), _T("cctvdba"), strConstructString, TRUE);
	RegGetString(_T("DatabaseName"), _T(""), strDatabaseName, TRUE);

	RegWriteString(_T("DatabaseType"), strDatabaseType);
	RegWriteString(_T("ServerName"), strServerName);
	RegWriteString(_T("UserID"), strUserID);
	RegWriteString(_T("Password"), strConstructString);
	RegWriteString(_T("DatabaseName"), strDatabaseName);
}

BOOL CCheckDbConnect::RegGetString(LPCTSTR lpszValueName,LPCTSTR lpszDefault,LPTSTR lpszValue, BOOL bIsBackupDatabase, BOOL bAutoCreate)
{
	LPCTSTR lpszSubKey;
	HKEY hKeyParent = HKEY_LOCAL_MACHINE;
	if (bIsBackupDatabase)
	{
		lpszSubKey = _T("SOFTWARE\\Sobey\\Public\\StdioDBManageBackup");
	}
	else
	{
		lpszSubKey = _T("SOFTWARE\\Sobey\\Public\\StdioDBManage");
	}

	HKEY  hKey = NULL;
	DWORD dwBufLen = 100;
	DWORD dwRegType = REG_SZ;
	// 打开该注册表键
	long lRet = ::RegOpenKeyEx(
		hKeyParent, 
		lpszSubKey, 
		0, 
		KEY_ALL_ACCESS, 
		&hKey
		);
	if (lRet != ERROR_SUCCESS)
	{
		// 打开失败
		// 创建该注册表键
		DWORD dwDisposition;
		lRet = ::RegCreateKeyEx(
			hKeyParent, 
			lpszSubKey, 
			0, 
			REG_NONE, 
			REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, 
			NULL, 
			&hKey, 
			&dwDisposition
			);
		if (lRet == ERROR_SUCCESS)
		{
			// 创建成功
			// 写入缺省值
			if(bAutoCreate)
			{
				lRet = ::RegSetValueEx(
					hKey, 
					lpszValueName, 
					NULL, 
					REG_SZ, 
					(LPBYTE) lpszDefault, 
					(_tcslen(lpszDefault) + 1) * sizeof(TCHAR)
					);
			}
			// 关闭该注册表键
			::RegCloseKey(hKey);
		}
		_tcscpy(lpszValue, lpszDefault);
		return FALSE;
	}

	// 打开成功
	lRet = ::RegQueryValueEx(
		hKey, 
		lpszValueName, 
		NULL, 
		&dwRegType, 
		(LPBYTE) lpszValue, 
		&dwBufLen
		);
	if (lRet != ERROR_SUCCESS)
	{
		// 读取失败
		// 写入缺省值
		if(bAutoCreate)
		{
			lRet = ::RegSetValueEx(
				hKey, 
				lpszValueName, 
				NULL, 
				REG_SZ, 
				(LPBYTE) lpszDefault, 
				(_tcslen(lpszDefault) + 1) * sizeof(TCHAR)
				);
		}
		// 关闭该注册表键
		::RegCloseKey(hKey);
		_tcscpy(lpszValue, lpszDefault);
		return FALSE;
	}

	// 读取成功
	::RegCloseKey(hKey);
	if (dwRegType != REG_SZ)
	{
		_tcscpy(lpszValue, lpszDefault);
	}
	return TRUE;
}

void CCheckDbConnect::RegWriteString(LPCTSTR lpszValueName, LPCTSTR lpszValue, BOOL bIsBackupDatabase /* = FALSE */)
{
	LPCTSTR lpszSubKey;
	HKEY hKeyParent = HKEY_LOCAL_MACHINE;
	if (bIsBackupDatabase)
	{
		lpszSubKey = _T("SOFTWARE\\Sobey\\Public\\StdioDBManageBackup");
	}
	else
	{
		lpszSubKey = _T("SOFTWARE\\Sobey\\Public\\StdioDBManage");
	}

	HKEY  hKey = NULL;
	DWORD dwBufLen = 100;
	DWORD dwRegType = REG_SZ;
	// 打开该注册表键
	long lRet = ::RegOpenKeyEx(
		hKeyParent, 
		lpszSubKey, 
		0, 
		KEY_ALL_ACCESS, 
		&hKey
		);
	if (lRet != ERROR_SUCCESS)
	{
		// 打开失败
		// 创建该注册表键
		DWORD dwDisposition;
		lRet = ::RegCreateKeyEx(
			hKeyParent, 
			lpszSubKey, 
			0, 
			REG_NONE, 
			REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, 
			NULL, 
			&hKey, 
			&dwDisposition
			);
		if (lRet != ERROR_SUCCESS)
		{
			return;
		}
	}
	lRet = ::RegSetValueEx(
		hKey, 
		lpszValueName, 
		NULL, 
		REG_SZ, 
		(LPBYTE) lpszValue, 
		(_tcslen(lpszValue) + 1) * sizeof(TCHAR)
		);
	// 关闭该注册表键
	::RegCloseKey(hKey);
	return;
}

