// StdioDB.cpp : Implementation of CStdioDB
#include "stdafx.h"
#include "StdioDBManage.h"
#include "StdioDB.h"
#include "oledb.h"
#include "checkdbconnect.h"


/////////////////////////////////////////////////////////////////////////////
// CStdioDB

#define DBCHECK_NULL		-1
#define DBCHECK_CHECKING	1
#define DBCHECK_CHECKED		2
#define DBCHECK_CONNECTING	3
#define DBCHECK_CONNECTED	4

HANDLE	g_hDbConnectCheckThread = NULL;
DWORD g_dwLastConnectTime = 0;
DWORD g_dwLastCheckTime = 0;
BOOL g_bIsConnect = TRUE;
int g_iWorkStatic = -1;
int g_iStdioDBID = 10;


BOOL IsConnectDb()
{
	if (g_bIsConnect == FALSE)
	{
		if (g_iWorkStatic == DBCHECK_CONNECTING)
		{
			// ������ݿ��������ӣ���������ʱ�䳬��2000,��ô��Ϊ�޷������ϡ����û����2000,�ȴ���2000,������û��ϣ�������ϡ�
			DWORD dw = GetTickCount() - g_dwLastConnectTime;

			if (dw > 2000)
				return FALSE;
			else
			{
				Sleep(dw);
				return g_bIsConnect;
			}
		}
		else if (g_iWorkStatic == DBCHECK_CONNECTED)
			return FALSE;
	}
	else
	{
		if (g_iWorkStatic == DBCHECK_CHECKING)
		{
			DWORD dw = GetTickCount() - g_dwLastCheckTime;
			if (dw > 1000)
				return FALSE;
			else
			{
				Sleep(dw);
				return g_bIsConnect;
			}
		}
		else if (g_iWorkStatic == DBCHECK_CHECKED)
			return TRUE;
	}
}

void CheckDbConnect()
{
	g_iWorkStatic = DBCHECK_NULL;
	CCheckDbConnect db;

	HRESULT hr = db.ConnectDatabase();

	if (hr == S_OK)
		g_bIsConnect = TRUE;
	else
		g_bIsConnect = FALSE;

	while (TRUE)
	{
		if (g_bIsConnect == FALSE)
		{
			g_dwLastConnectTime = GetTickCount();
			g_iWorkStatic = DBCHECK_CONNECTING;
			hr = db.PrepareConnectPtr();
			g_iWorkStatic = DBCHECK_CONNECTED;

			if (hr == S_OK)
				g_bIsConnect = TRUE;
			else
				g_bIsConnect = FALSE;
		}
		else
		{
			g_dwLastCheckTime = ::GetTickCount();
			g_iWorkStatic = DBCHECK_CHECKING;
			BOOL b = db.CheckStatus();
			g_iWorkStatic = DBCHECK_CHECKED;

			if (b == FALSE)
				g_bIsConnect = FALSE;
			else
				g_bIsConnect = TRUE;
		}
		Sleep(1000);
	}
}


DWORD WINAPI CheckDbConnectThread(LPVOID lpVoid)
{
	CheckDbConnect();
	return 0;
}


#include "Mmsystem.h"
#pragma comment ( lib, "Winmm.lib" )

#define  WHERE_LENGTH	15

static BOOL g_bIsUserBackupDatebase = FALSE;
static DWORD g_dwFaildTime = 0;

typedef struct
{
	int		nTableID;
	TCHAR	lpszName[31];
}LOB_DATA_DEF;

LOB_DATA_DEF g_lpszHavLobTable[] = 
{
	1,_T("")
};

LOB_DATA_DEF g_lpszHavLobField[] = 
{
	1,_T("")
};

//����Ƿ��д��ֶ�
//TRUE�����ܺ��д��ֶΣ�FALSE�����Բ������ֶ�
BOOL CheckHaveLOB(BSTR bstrSql)
{
	long i;
	LPTSTR lpszBuf,lpszTable,p;
	lpszBuf = new TCHAR[::SysStringLen(bstrSql) + 1];
	_tcscpy(lpszBuf,bstrSql);
	_tcsupr(lpszBuf);
	p = _tcsstr(lpszBuf,_T("*"));

	lpszTable = _tcsstr(lpszBuf,_T("FROM"));
	lpszTable = lpszTable + 4;
	p = _tcsstr(lpszTable,_T("WHERE"));
	if(p) *p = 0;
	p = _tcsstr(lpszTable,_T("GROUP"));
	if(p) *p = 0;
	p = _tcsstr(lpszTable,_T("HAVING"));
	if(p) *p = 0;
	p = _tcsstr(lpszTable,_T("UNION"));
	if(p) *p = 0;
	p = _tcsstr(lpszTable,_T("MINUS"));
	if(p) *p = 0;
	p = _tcsstr(lpszTable,_T("INTERSECT"));
	if(p) *p = 0;

	i = 0;
	while(lstrlen(g_lpszHavLobTable[i].lpszName) > 5)
	{
		p = _tcsstr(lpszTable,g_lpszHavLobTable[i].lpszName);
		if(p)
		{
			p += _tcslen(g_lpszHavLobTable[i].lpszName);
			if(*p != _T('_') && (*p < _T('A') || *p > _T('Z')))
			{
				delete lpszBuf;
				return TRUE;
			}
		}
		i++;
	}

	delete lpszBuf;
	return FALSE;
}

BOOL CheckHaveLOBField(_Recordset *pRs)
{
	BOOL bFind = FALSE;
	long i = 0,j,k,nTableID,nCount;
	LPCTSTR lpszTableName;
	_bstr_t bstrTableName,bstrColName;
	try
	{
		nCount = pRs->Fields->GetCount();
		bstrTableName = pRs->Fields->GetItem(0L)->Properties->GetItem(L"BASETABLENAME")->Value;
		lpszTableName = (LPCTSTR)(BSTR)bstrTableName;
		while(lstrlen(g_lpszHavLobTable[i].lpszName) > 5)
		{
			if(lstrcmp(lpszTableName,g_lpszHavLobTable[i].lpszName) == 0)
			{
				nTableID = g_lpszHavLobTable[i].nTableID;
				j = nTableID - 1;
				while(TRUE)
				{
					if(g_lpszHavLobField[j].nTableID == nTableID)
					{
						bFind = TRUE;
						for(k = 0;k < nCount;k++)
						{
							bstrColName = pRs->Fields->GetItem(k)->Properties->GetItem(L"BASECOLUMNNAME")->Value;
							if(lstrcmp((LPCTSTR)bstrColName,g_lpszHavLobField[j].lpszName) == 0)
							{
								return TRUE;
							}
						}
					}
					else if(bFind)
					{
						return FALSE;
					}
					if(j > 40)
						return FALSE;
					j++;
				}
				return FALSE;
			}
			i++;
		}
	}
	catch(_com_error e)
	{
		DWORD dwSize;
		TCHAR __buf[40];
		TCHAR __Userbuf[255];
		TCHAR __Compbuf[255];
		CComBSTR bstrRemark;
		dwSize = 255;
		GetUserName(__Userbuf,&dwSize);
		dwSize = 255;
		GetComputerName(__Compbuf,&dwSize);
		wsprintf(__buf, _T("(0X%x)"), e.Error());
		CComBSTR bstrErrorDescription = __buf;
		bstrErrorDescription += (TCHAR *)e.Description();
		bstrRemark = L"";
		WriteLog(bstrErrorDescription, CComBSTR(__Compbuf), 0, CComBSTR(_T("CheckHaveLOBField")), CComBSTR(__FILE__), __LINE__, CComBSTR(__Userbuf), bstrRemark);
	}
	return FALSE;
}

CStdioDB::CStdioDB()
{
	m_iStdioDBID = g_iStdioDBID;
	g_iStdioDBID ++;

	if (g_hDbConnectCheckThread == NULL)
		g_hDbConnectCheckThread = ::CreateThread(NULL,0,CheckDbConnectThread,this,0,0);

	m_lDatabaseType = DB_ORACLE;
	m_spAdoCon = NULL;
	m_lDBRunMode = 0;
	m_lWriteLogFlag = 0;
	m_lWriteEfficiencyLog = 0;
	m_bIsBackupDatabase = FALSE;
	m_bBeginTrans = FALSE;

	ConnectDatabase();
}

CStdioDB::~CStdioDB()
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

HRESULT CStdioDB::Activate()
{
	return S_OK;
} 

BOOL CStdioDB::CanBePooled()
{
	return TRUE;
} 

void CStdioDB::Deactivate()
{
} 

STDMETHODIMP CStdioDB::Construct(IDispatch *pDisp)
{
	return S_OK;
}

STDMETHODIMP CStdioDB::EnlistResource(BOOL bEnlist)
{
	return S_OK;
}

STDMETHODIMP CStdioDB::GetRecordset(BSTR bstrSQL, _Recordset **ppRs)
{
	HRESULT hr;
	LARGE_INTEGER liStart;
	try
	{
		if(m_lWriteEfficiencyLog)
		{
			QueryPerformanceCounter(&liStart);
		}
		hr = GetRecordsetCore(bstrSQL, ppRs);
		if(m_lWriteEfficiencyLog)
		{
			long lTime,lRecordCount;
			TCHAR lpszBuffer [256];
			LARGE_INTEGER liFrequency,liEnd;
			QueryPerformanceCounter(&liEnd);
			QueryPerformanceFrequency(&liFrequency);
			lTime = (long)((liEnd.QuadPart - liStart.QuadPart)*1000/liFrequency.QuadPart);

			if (SUCCEEDED(hr) && ppRs && *ppRs)
				lRecordCount = (*ppRs)->RecordCount;
			else
				lRecordCount = 0;

			_stprintf(lpszBuffer,_T("hr=%x, ����ʱ�䣺%d ms,��ѯ���������%d"), hr, lTime,lRecordCount);
			ReprotError(lpszBuffer, L"CStdioDB::GetRecordset", __FILE__,__LINE__, bstrSQL);
		}

		// һ��ִ�гɹ����ͽ�ȫ��ʱ�������Ϊ0
		if (SUCCEEDED(hr))
			g_dwFaildTime = 0;
	}
	catch(_com_error e)
	{
		return ReprotError(&e, CComBSTR("CStdioDB::GetRecordset"),__FILE__,__LINE__,bstrSQL);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), CComBSTR("CStdioDB::GetRecordset"),__FILE__,__LINE__,bstrSQL);
		return E_FAIL;
	} 
	return hr;
}

HRESULT CStdioDB::GetRecordsetCore(BSTR bstrSQL, _Recordset **ppRs)
{
	BOOL bHaveLOB = FALSE;
	if(m_lDBRunMode&&m_lDatabaseType == DB_ORACLE)
	{
		bHaveLOB = CheckHaveLOB(bstrSQL);
	}
	if(m_lDBRunMode != 2||m_lDatabaseType != DB_ORACLE||(m_lDBRunMode == 2&&m_lDatabaseType == DB_ORACLE&&!bHaveLOB))
	{
		HRESULT hr;
		BOOL bCreateRs;
		long lCount = 0;
		_RecordsetPtr spAdoRs;

REGETRECORDSET:		
		bCreateRs = FALSE;
		lCount++;
		if(lCount > 3)
		{
			ReprotError(CComBSTR(L"���ݿ������⣬�������ݿ⣡"), CComBSTR("CStdioDB::GetRecordset"),__FILE__,__LINE__,bstrSQL);
			return E_FAIL;
		}

		// ������������������ݿ��ָ��
		hr = PrepareConnectPtr();
		if(FAILED(hr)) return hr;
		try
		{
			hr = spAdoRs.CreateInstance(__uuidof(Recordset));
			if(FAILED(hr)) return hr;
			bCreateRs = TRUE;
			spAdoRs->PutCursorLocation(adUseClientBatch);
			spAdoRs->Open(bstrSQL, m_spAdoCon.GetInterfacePtr(), adOpenStatic, adLockBatchOptimistic, -1);
			VARIANT var;
			var.vt = VT_DISPATCH;
			var.pdispVal = NULL;
			spAdoRs->PutActiveConnection(var);
			*ppRs = spAdoRs.Detach();
		}
		catch(_com_error e)
		{
			if(FAILED(ReConnect(&e)))
			{
				return ReprotError(&e, CComBSTR("CStdioDB::GetRecordset"),__FILE__,__LINE__,bstrSQL);
			}
			else
			{
				if(bCreateRs)
					spAdoRs.Release();
				goto REGETRECORDSET;
			}
		}
		catch(...)
		{
			ReprotError(_T("������ͨ�쳣"), CComBSTR("CStdioDB::GetRecordset"),__FILE__,__LINE__,bstrSQL);
			return E_FAIL;
		}
	}
	else
	{
		HRESULT hr;
		VARIANT var;
		BOOL bHaveLobField;
		long lCount = 0;
		LPTSTR lpszBuf = NULL;
		_CommandPtr spAdoCmd = NULL;
		_ParameterPtr spAdoClob = NULL;
		_RecordsetPtr spAdoRs = NULL,spCpRs = NULL;
		VariantInit(&var);
REGETRECORDSETWITHLOB:
		lCount++;
		if(lCount > 3)
		{
			ReprotError(L"���ݿ������⣬�������ݿ⣡", L"CStdioDB::GetRecordset",__FILE__,__LINE__,bstrSQL);
			return E_FAIL;
		}

		// ������������������ݿ��ָ��
		hr = PrepareConnectPtr();
		if(FAILED(hr)) return hr;
		try
		{
			bHaveLobField = TRUE;
			lpszBuf = new TCHAR[::SysStringLen(bstrSQL) + WHERE_LENGTH];
			if(GetRelationSql(bstrSQL,lpszBuf))
			{
				hr = spAdoRs.CreateInstance(__uuidof(Recordset));
				if(FAILED(hr)) return hr;
				spAdoRs->PutCursorLocation(adUseClientBatch);
				spAdoRs->Open(lpszBuf, m_spAdoCon.GetInterfacePtr(), adOpenStatic, adLockBatchOptimistic, -1);

				var.vt = VT_DISPATCH;
				var.pdispVal = NULL;
				spAdoRs->PutActiveConnection(var);
				bHaveLobField = CheckHaveLOBField(spAdoRs);
				if(bHaveLobField)
				{
					VariantInit(&var);
					//�µķ������Խ��CLOB�ڴ�й©������
					m_spAdoCon->PutCursorLocation(adUseClientBatch);
					hr = spAdoCmd.CreateInstance(__uuidof(Command));
					if(FAILED(hr)) return hr;
					spAdoCmd->ActiveConnection = m_spAdoCon;
					spAdoClob = spAdoCmd->CreateParameter(L"prCLOB",adLongVarChar,adParamOutput,100000);
					hr = spAdoCmd->Parameters->Append(spAdoClob);
					spAdoCmd->Properties->GetItem(L"SPPrmsLOB")->Value = 1L;

					spAdoCmd->CommandText = bstrSQL;
					spAdoCmd->CommandType = adCmdText;
					spCpRs = spAdoCmd->Execute(&var,NULL,adCmdText);
					m_spAdoCon->PutCursorLocation(adUseServer);

					if(spAdoRs)
					{
						CopyRecordset(spCpRs, spAdoRs,bstrSQL,lpszBuf);
						*ppRs = spAdoRs.Detach();
					}
					else
					{
						var.vt = VT_DISPATCH;
						var.pdispVal = NULL;
						spCpRs->PutActiveConnection(var);
						*ppRs = spCpRs.Detach();
					}
				}
				else
				{
					hr = spAdoRs.CreateInstance(__uuidof(Recordset));
					if(FAILED(hr)) return hr;
					spAdoRs->PutCursorLocation(adUseClientBatch);
					spAdoRs->Open(bstrSQL, m_spAdoCon.GetInterfacePtr(), adOpenStatic, adLockBatchOptimistic, -1);
					var.vt = VT_DISPATCH;
					var.pdispVal = NULL;
					spAdoRs->PutActiveConnection(var);
					*ppRs = spAdoRs.Detach();
				}
			}
			else
			{
				VariantInit(&var);
				//�µķ������Խ��CLOB�ڴ�й©������
				m_spAdoCon->PutCursorLocation(adUseClientBatch);
				hr = spAdoCmd.CreateInstance(__uuidof(Command));
				if(FAILED(hr)) return hr;
				spAdoCmd->ActiveConnection = m_spAdoCon;
				spAdoClob = spAdoCmd->CreateParameter(L"prCLOB",adLongVarChar,adParamOutput,100000);
				hr = spAdoCmd->Parameters->Append(spAdoClob);
				spAdoCmd->Properties->GetItem(L"SPPrmsLOB")->Value = 1L;

				spAdoCmd->CommandText = bstrSQL;
				spAdoCmd->CommandType = adCmdText;
				spCpRs = spAdoCmd->Execute(&var,NULL,adCmdText);
				m_spAdoCon->PutCursorLocation(adUseServer);
				*ppRs = spCpRs.Detach();
			}
			VariantClear(&var);
		}
		catch(_com_error e)
		{
			m_spAdoCon->PutCursorLocation(adUseServer);
			if(lpszBuf)
			{
				ReprotError(&e, L"CStdioDB::GetRecordset",__FILE__,__LINE__,CComBSTR(lpszBuf));
				delete [] lpszBuf;
				lpszBuf = NULL;
			}
			if(spAdoCmd)	spAdoCmd.Release();
			if(spAdoClob)	spAdoClob.Release();
			if(spAdoRs)		spAdoRs.Release();
			if(spCpRs)		spAdoRs.Release();
			VariantClear(&var);
			if(FAILED(ReConnect(&e)))
			{
				ReprotError(&e, L"CStdioDB::GetRecordset",__FILE__,__LINE__,bstrSQL);
				return E_FAIL;
			}
			else
			{
				goto REGETRECORDSETWITHLOB;
			}
		}
		catch(...)
		{
			if(spAdoCmd)	spAdoCmd.Release();
			if(spAdoClob)	spAdoClob.Release();
			if(spCpRs)
			{
				long lState;
				spCpRs->get_State(&lState);
				if(lState != adStateClosed)
				{
					spCpRs->Close();
				}
				spCpRs.Release();
			}
			if(lpszBuf)
			{
				delete [] lpszBuf;
				lpszBuf = NULL;
			}
			ReprotError(_T("������ͨ�쳣"), L"CStdioDB::GetRecordset",__FILE__,__LINE__,bstrSQL);
			return E_FAIL;
		}

		if(spAdoCmd)	spAdoCmd.Release();
		if(spAdoClob)	spAdoClob.Release();
		if(spCpRs)
		{
			long lState;
			spCpRs->get_State(&lState);
			if(lState != adStateClosed)
			{
				spCpRs->Close();
			}
			spCpRs.Release();
		}
		if(lpszBuf)
		{
			delete [] lpszBuf;
			lpszBuf = NULL;
		}
	}

	return S_OK;
}

STDMETHODIMP CStdioDB::ExecuteCommand(BSTR bstrCmd, unsigned long *pulAffectCount)
{
	HRESULT hr;
	LARGE_INTEGER liStart;
	try
	{
		*pulAffectCount = 0;
		if(m_lWriteEfficiencyLog)
		{
			QueryPerformanceCounter(&liStart);
		}
		hr = ExecuteCommandCore(bstrCmd,pulAffectCount);
		if(m_lWriteEfficiencyLog)
		{
			long lTime,lAffectCount;
			TCHAR lpszBuffer [256];
			LARGE_INTEGER liFrequency,liEnd;
			QueryPerformanceCounter(&liEnd);
			QueryPerformanceFrequency(&liFrequency);
			lTime = (long)((liEnd.QuadPart - liStart.QuadPart)*1000/liFrequency.QuadPart);

			lAffectCount = *pulAffectCount;
			_stprintf(lpszBuffer,_T("hr=%x, ����ʱ�䣺%d ms,Ӱ������������%d"),hr,lTime,lAffectCount);
			ReprotError(lpszBuffer, L"CStdioDB::ExecuteCommand", __FILE__,__LINE__, bstrCmd);
		}

		// һ��ִ�гɹ����ͽ�ȫ��ʱ�������Ϊ0
		if (SUCCEEDED(hr))
			g_dwFaildTime = 0;
	}
	catch(_com_error e)
	{
		return ReprotError(&e, CComBSTR("CStdioDB::ExecuteCommand"),__FILE__,__LINE__,bstrCmd);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), CComBSTR("CStdioDB::ExecuteCommand"),__FILE__,__LINE__,bstrCmd);
		return E_FAIL;
	}
	return hr;
}

HRESULT CStdioDB::ExecuteCommandCore(BSTR bstrCmd, unsigned long *pulAffectCount)
{
	HRESULT hr;
	_variant_t var;
	long lCount = 0;
REEXECCOMMAND:
	lCount++;
	if(lCount > 3)
	{
		ReprotError(L"���ݿ������⣬�������ݿ⣡", L"CStdioDB::ExecuteCommand",__FILE__,__LINE__,bstrCmd);
		return E_FAIL;
	}

	// ������������������ݿ��ָ��
	hr = PrepareConnectPtr();
	if(FAILED(hr)) return hr;

	try
	{
		if (m_lDatabaseType == DB_DB2)
		{
			if(IsQuerySql(bstrCmd))
			{
				_RecordsetPtr pRs;
				GetRecordset(bstrCmd,&pRs);
				if(pRs)
					*pulAffectCount = pRs->GetRecordCount();
				else
					*pulAffectCount = 0;
			}
			else
			{
				_RecordsetPtr pRs = m_spAdoCon->Execute(bstrCmd, &var, adCmdText | adExecuteNoRecords);
				if(var.vt != VT_NULL)
				{
					var.ChangeType(VT_I4);
				}

				*pulAffectCount = abs(var.lVal);
			}
		}
		else
		{
			_RecordsetPtr pRs = m_spAdoCon->Execute(bstrCmd, &var, adCmdText | adExecuteNoRecords);
			if(var.vt != VT_NULL)
			{
				var.ChangeType(VT_I4);
			}

			if(m_lDatabaseType==DB_ORACLE)
				*pulAffectCount = var.lVal;
			else
				*pulAffectCount = abs(var.lVal);
		}
	}
	catch(_com_error e)
	{
		if(FAILED(ReConnect(&e)))
		{
			ReprotError(&e, L"CStdioDB::ExecuteCommand",__FILE__,__LINE__,bstrCmd);
			return E_FAIL;
		}
		else
		{
			var.Clear();
			goto REEXECCOMMAND;
		}
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::ExecuteCommand",__FILE__,__LINE__,bstrCmd);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CStdioDB::GetNextID(BSTR bstrTableName, long *plNextID)
{
	if(m_lDatabaseType!=DB_SQLSERVER)
		return E_FAIL;

	_variant_t varIn,varOut;
	_ParameterPtr spAdoParam,spAdoParamOut;
	_CommandPtr spAdoCmd;
	HRESULT hr = 0;
	try
	{
		// ������������������ݿ��ָ��
		hr = PrepareConnectPtr();
		if(FAILED(hr)) return hr;

		hr = spAdoCmd.CreateInstance(__uuidof(Command));
		if(FAILED(hr)) return hr;
		spAdoCmd->ActiveConnection = m_spAdoCon;
		spAdoCmd->CommandText = _T("next_id");
		spAdoCmd->CommandType = adCmdStoredProc;

		_bstr_t bstr_param = bstrTableName;
		varIn.vt = VT_BSTR;
		varIn.bstrVal = bstr_param;
		spAdoParam = spAdoCmd->CreateParameter(_T("TABLENAME"),adBSTR,adParamInput,bstr_param.length(),varIn);
		spAdoCmd->Parameters->Append(spAdoParam);
		spAdoParam->Value = varIn;

		spAdoParamOut = spAdoCmd->CreateParameter(_T("value"),adInteger,adParamOutput,sizeof(int));		
		spAdoCmd->Parameters->Append(spAdoParamOut);

		spAdoCmd->Execute(NULL,NULL,adCmdStoredProc | adExecuteNoRecords); 

		varOut = spAdoParamOut->Value;

		varOut.ChangeType(VT_I4);

		*plNextID = varOut.lVal;
	}
	catch(_com_error e)
	{
		return ReprotError(&e, L"CStdioDB::GetNextID",__FILE__,__LINE__,bstrTableName);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::GetNextID",__FILE__,__LINE__,bstrTableName);
		return E_FAIL;
	}

	// һ��ִ�гɹ����ͽ�ȫ��ʱ�������Ϊ0
	g_dwFaildTime = 0;

	return S_OK;
}

STDMETHODIMP CStdioDB::UpdateRecordset(_Recordset *pRs)
{
	HRESULT hr;
	LARGE_INTEGER liStart;
	try
	{
		if(m_lWriteEfficiencyLog)
		{
			QueryPerformanceCounter(&liStart);
		}
		hr = UpdateRecordsetCore(pRs);
		if(m_lWriteEfficiencyLog)
		{
			long lTime;
			_bstr_t bstrTableName;
			TCHAR lpszBuffer [100];
			LARGE_INTEGER liFrequency,liEnd;
			QueryPerformanceCounter(&liEnd);
			QueryPerformanceFrequency(&liFrequency);
			bstrTableName = pRs->Fields->GetItem(0L)->Properties->GetItem(L"BASETABLENAME")->Value;
			lTime = (long)((liEnd.QuadPart - liStart.QuadPart)*1000/liFrequency.QuadPart);

			_stprintf(lpszBuffer,_T("����ʱ�䣺%d ms,�޸ı�%s,�޸�������%d"),lTime,(LPCTSTR)bstrTableName,pRs->RecordCount);
			ReprotError(lpszBuffer, L"CStdioDB::UpdateRecordset", __FILE__,__LINE__, NULL);
		}

		// һ��ִ�гɹ����ͽ�ȫ��ʱ�������Ϊ0
		if (SUCCEEDED(hr))
			g_dwFaildTime = 0;
	}
	catch(_com_error e)
	{
		return ReprotError(&e, CComBSTR("CStdioDB::UpdateRecordset"),__FILE__,__LINE__,NULL);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), CComBSTR("CStdioDB::UpdateRecordset"),__FILE__,__LINE__,NULL);
		return E_FAIL;
	}
	return hr;
}

HRESULT CStdioDB::UpdateRecordsetCore(_Recordset *pRs)
{
	HRESULT hr;
	BOOL bHaveLOB = FALSE;
	if(m_lDBRunMode&&m_lDatabaseType == DB_ORACLE)
	{
		bHaveLOB = CheckHaveLOBField(pRs);
	}

	if((m_lDBRunMode != 2||m_lDatabaseType != DB_ORACLE)||(m_lDBRunMode == 2&&m_lDatabaseType == DB_ORACLE&&!bHaveLOB))
	{
		long lCount = 0;

REUPDATERECORDSET:
		lCount++;
		if(lCount > 3)
		{
			ReprotError(L"���ݿ������⣬�������ݿ⣡", L"CStdioDB::UpdateRecordset",__FILE__,__LINE__,L"UpdateRecordset");
			return E_FAIL;
		}

		// ������������������ݿ��ָ��
		hr = PrepareConnectPtr();
		if(FAILED(hr)) return hr;

		VARIANT var;
		VariantInit(&var);
		try
		{
			pRs->PutActiveConnection(m_spAdoCon.GetInterfacePtr());
			pRs->UpdateBatch(adAffectAll);

			var.vt = VT_DISPATCH;
			var.pdispVal = NULL;
			pRs->PutActiveConnection(var);
			VariantClear(&var);
		}
		catch(_com_error e)
		{
			CComBSTR bstrRemark;
			bstrRemark=GetRemark(pRs);
			VariantClear(&var);
			if(e.Error() == 0x80040E38)
			{
				try
				{
					_variant_t var((long)adFilterConflictingRecords);
					pRs->PutFilter(&var);
					pRs->Resync(adAffectGroup, adResyncUnderlyingValues);
					pRs->UpdateBatch(adAffectGroup);

					var.vt = VT_DISPATCH;
					var.pdispVal = NULL;
					pRs->PutActiveConnection(var);
					VariantClear(&var);
				}
				catch(_com_error e1)
				{
					VariantClear(&var);
					return ReprotError(&e, CComBSTR("CStdioDB::UpdateRecordset"),__FILE__,__LINE__,bstrRemark);
				}
				catch(...)
				{
					ReprotError(_T("������ͨ�쳣"), L"CStdioDB::UpdateRecordset",__FILE__,__LINE__,bstrRemark);
					return E_FAIL;
				}
				return S_OK;
			}
			var.vt = VT_DISPATCH;
			var.pdispVal = NULL;
			pRs->PutActiveConnection(var);
			VariantClear(&var);

			if(FAILED(ReConnect(&e)))
			{
				return ReprotError(&e, CComBSTR("CStdioDB::UpdateRecordset"),__FILE__,__LINE__,bstrRemark);
			}
			else
			{
				goto REUPDATERECORDSET;
			}

			return ReprotError(&e, CComBSTR("CStdioDB::UpdateRecordset"),__FILE__,__LINE__,bstrRemark);
		}
		catch(...)
		{
			ReprotError(_T("������ͨ�쳣"), L"CStdioDB::UpdateRecordset",__FILE__,__LINE__);
			return E_FAIL;
		}
		return S_OK;
	}
	else
	{
		return UpdateRecordsetWithLOB(pRs);
	}

	return E_FAIL;
}

STDMETHODIMP CStdioDB::DeviateTransaction()
{
	if(m_spAdoCon == NULL)
		return E_FAIL;

	try
	{
		long lState = 0;
		m_spAdoCon->get_State(&lState);
		if(lState == adStateClosed)
			return E_FAIL;
		m_spAdoCon->BeginTrans ();
		m_bBeginTrans = TRUE;
	}
	catch(_com_error e)
	{
		return ReprotError(&e, L"CStdioDB::DeviateTransaction", __FILE__,__LINE__);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::DeviateTransaction",__FILE__,__LINE__);
		return E_FAIL;
	}

/*	if(m_spObjCtx->IsInTransaction())
	{
	return EnlistResource(FALSE);
	}*/
	return S_OK;
}

STDMETHODIMP CStdioDB::ManualCommit()
{
	/*	if(m_spObjCtx->IsInTransaction())
	{
	m_spObjCtx->SetComplete();
	}
	*/
	if(m_spAdoCon == NULL)
		return E_FAIL;
	try
	{
		long lState = 0;
		m_spAdoCon->get_State(&lState);
		if(lState == adStateClosed)
			return E_FAIL;

		m_spAdoCon->CommitTrans ();
		m_bBeginTrans = FALSE;
	}
	catch(_com_error e)
	{
		return ReprotError(&e, L"CStdioDB::ManualCommit", __FILE__,__LINE__);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::ManualCommit",__FILE__,__LINE__);
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CStdioDB::ReprotError(_com_error *pError, LPWSTR lpswErrorMothod, LPCSTR lpszFileName, long lLineNo, LPWSTR lpswRemark)
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
		WriteLog(bstrErrorDescription, bstrComputer, m_iStdioDBID, bstrErrorMothod, bstrFileName, lLineNo, bstrAuthorName, bstrRemark);
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

// �л�����������ϢΪ�����������Ϣ
void CStdioDB::RegChangeString()
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

// ��ȡע����е��ַ���ֵ
BOOL CStdioDB::RegGetString(LPCTSTR lpszValueName,LPCTSTR lpszDefault,LPTSTR lpszValue, BOOL bIsBackupDatabase, BOOL bAutoCreate)
{
	LPCTSTR lpszSubKey;
	HKEY hKeyParent = HKEY_LOCAL_MACHINE;
	if (bIsBackupDatabase)
	{
		//lpszSubKey = _T("SOFTWARE\\Sobey\\Public\\StdioDBManageBackup");
		lpszSubKey = _T("SOFTWARE\\WOW6432Node\\Sobey\\Public\\StdioDBManageBackup");
	}
	else
	{
		//lpszSubKey = _T("SOFTWARE\\Sobey\\Public\\StdioDBManage");
		lpszSubKey = _T("SOFTWARE\\WOW6432Node\\Sobey\\Public\\StdioDBManage");
	}

//	RegDeleteKeyValue(HKEY_LOCAL_MACHINE,lpszSubKey,lpszValueName);
//	return FALSE;
	HKEY  hKey = NULL;
	DWORD dwBufLen = 100;
	DWORD dwRegType = REG_SZ;
	// �򿪸�ע����
	long lRet = ::RegOpenKeyEx(
		hKeyParent, 
		lpszSubKey, 
		0, 
		KEY_QUERY_VALUE, 
		&hKey
		);
	if (lRet != ERROR_SUCCESS)
	{
		// ��ʧ��
		// ������ע����
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
			// �����ɹ�
			// д��ȱʡֵ
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
			// �رո�ע����
			::RegCloseKey(hKey);
		}
		_tcscpy(lpszValue, lpszDefault);
		return FALSE;
	} 

	// �򿪳ɹ�
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
		// ��ȡʧ��
		// д��ȱʡֵ
		if(bAutoCreate)
		{
			::RegCloseKey(hKey);
			::RegOpenKeyEx(
				hKeyParent, 
				lpszSubKey, 
				0, 
				KEY_SET_VALUE, 
				&hKey
				);

			lRet = ::RegSetValueEx(
				hKey, 
				lpszValueName, 
				NULL, 
				REG_SZ, 
				(LPBYTE) lpszDefault, 
				(_tcslen(lpszDefault) + 1) * sizeof(TCHAR)
				);
		}
		// �رո�ע����
		::RegCloseKey(hKey);
		_tcscpy(lpszValue, lpszDefault);
		return FALSE;
	}

	// ��ȡ�ɹ�
	::RegCloseKey(hKey);
	if (dwRegType != REG_SZ)
	{
		_tcscpy(lpszValue, lpszDefault);
	}
	return TRUE;
}

// д��ע���
void CStdioDB::RegWriteString(LPCTSTR lpszValueName, LPCTSTR lpszValue, BOOL bIsBackupDatabase /* = FALSE */)
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
	// �򿪸�ע����
	long lRet = ::RegOpenKeyEx(
		hKeyParent, 
		lpszSubKey, 
		0, 
		KEY_ALL_ACCESS, 
		&hKey
		);
	if (lRet != ERROR_SUCCESS)
	{
		// ��ʧ��
		// ������ע����
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
	// �رո�ע����
	::RegCloseKey(hKey);
	return;
}

STDMETHODIMP CStdioDB::GetDatabaseType(long *plDatabaseType)
{
	*plDatabaseType = m_lDatabaseType;
	return S_OK;
}

//���ݴ�������ж��Ƿ�������������
STDMETHODIMP CStdioDB::ReConnect(_com_error *pError)
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

		// ��������߳��Ѿ��е������ˣ����Լ�Ҳ�е�����
		bIsBackupDatebase = FALSE;
		if (g_bIsUserBackupDatebase == TRUE && m_bIsBackupDatabase == FALSE)
		{
			bNeedReConnect = TRUE;
			bIsBackupDatebase = TRUE;
		}

		if(bNeedReConnect)
			goto ReConnectDB;

		hr = pError->Error();
		if(m_lDatabaseType == DB_ORACLE)
		{
			if(hr == E_POINTER)//��Чָ�룬��ʾ���ݿ�������Ч
			{
				m_spAdoCon = NULL;
				goto ReConnectDB;
			}
			//�����޷�����ִ�д˲������ڴ����������������ѱ��رջ���Ч��������Ҫ�������ӡ�
			//����ر�ʱ�������������������Ҫ�������ӡ�
			else if(hr == 0X800A0E7D||hr == 0X800A0E78)
			{
				goto ReConnectDB;
			}

			bstrError = pError->Description();
			pwstrError = (wchar_t *)bstrError;
			for(i = 0;i < 10&&pwstrError[i] != L':'&&pwstrError[i] != L'��';i++)
			{
				pwstrTemp[i] = pwstrError[i];
			}
			pwstrTemp[9] = 0;
			if(wcscmp(pwstrTemp,L"ORA-03114") == 0)//Oracle�����ѶϿ�
			{
				goto ReConnectDB;
			}
			else if(wcscmp(pwstrTemp,L"ORA-03113") == 0)//һ�ֳ�����Oracle���ӶϿ��Ĵ���
			{
				goto ReConnectDB;
			}
			else if(wcscmp(pwstrTemp,L"ORA-01012") == 0)//һ��Oracleû�е�¼�Ĵ���
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
			if(hr == 0x800a0e7d || hr == 0x80004005) // ���ݿ��������������
				goto ReConnectDB;
		}
		else if (m_lDatabaseType == DB_DB2)
		{
			//ִ��һ�ζ�ȡ��ǰʱ���SQL�����������ݿ��Ƿ����
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
			//ִ��һ�ζ�ȡ��ǰʱ���SQL�����������ݿ��Ƿ����
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
		ReprotError(&e, L"CStdioDB::ReConnect", __FILE__,__LINE__);
		return E_FAIL;
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::ReConnect",__FILE__,__LINE__);
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

		// ������ӳ���ʱ��Ҳ���ɹ������Զ��л����������ݿ�
		if (bIsBackupDatebase)
		{
			hr = ConnectDatabase(TRUE); // ����������߳��Ѿ��е������ˣ���ֱ�����ӱ��⣬�������3�ζ�ʧ�ܣ��򷵻�E_FAIL
			if (SUCCEEDED(hr))
			{
				// �޸�����������ϢΪ����
				RegChangeString();
				g_bIsUserBackupDatebase = TRUE;
				m_bIsBackupDatabase = TRUE;
				ReprotError(L"�л����������ݿ�ɹ���", L"CStdioDB::ReConnect", __FILE__,__LINE__, NULL);
			}
		}
		else
		{
			hr = ConnectDatabase();
			if (CheckStatus())
			{
				g_dwFaildTime = 0;
			}
			else
			{
				DWORD dwCurrentTime = timeGetTime();
				TCHAR strDelayTiem[20];
				RegGetString(_T("DelayTime"), _T("0"), strDelayTiem,FALSE,TRUE);
				long lDelayTime = _ttol(strDelayTiem);
				if (lDelayTime != 0 && g_dwFaildTime != 0 && abs(dwCurrentTime - g_dwFaildTime) > lDelayTime*1000) // �����жϳ����趨ʱ�䣬���Զ����ӱ������ݿ�
				{
					hr = ConnectDatabase(TRUE);
					if (SUCCEEDED(hr))
					{
						// �޸�����������ϢΪ����
						RegChangeString();
						g_bIsUserBackupDatebase = TRUE;
						m_bIsBackupDatabase = TRUE;
						ReprotError(L"�л����������ݿ�ɹ���", L"CStdioDB::ReConnect", __FILE__,__LINE__, NULL);
					}
				}
			}
		}
		return hr;
	}
	catch(_com_error e)
	{
		ReprotError(&e, L"CStdioDB::ReConnect", __FILE__,__LINE__);
		return E_FAIL;
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::ReConnect",__FILE__,__LINE__);
		return E_FAIL;
	}
	return E_FAIL;
}

STDMETHODIMP CStdioDB::ConnectDatabase(BOOL bIsBackupDatabase /* = FALSE */)
{
	/*if (IsConnectDb() == FALSE)
	{
		ReprotError(_T("IsConnectDb() == FALSE"), L"CStdioDB::ConnectDatabase",__FILE__,__LINE__);
		return E_FAIL;
	}*/

	HRESULT hr;
	long lState;
	try
	{
		if (g_dwFaildTime == 0)
			g_dwFaildTime = timeGetTime();

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
			ReprotError(L"�������Connectionʧ�ܣ�", L"CStdioDB::ConnectDatabase", __FILE__,__LINE__, NULL);
			m_spAdoCon.Release();
			return E_FAIL;
		}
		if(m_spAdoCon == NULL)
		{
			ReprotError(L"�������Connection����ָ��Ϊ�գ�", L"CStdioDB::ConnectDatabase", __FILE__,__LINE__, NULL);
			return E_FAIL;
		}
		TCHAR strDatabaseType[100],strServerName[100],strUserID[100],strConstructString[100],strDatabaseName[100],strConnectionString[255],strDBRunMode[20],strWriteLogFlag[20];
		TCHAR strPort[100] = {0};

		if(bIsBackupDatabase)
		{
			if(!RegGetString(_T("ServerName"), _T("cctvdb"), strServerName, bIsBackupDatabase))
			{
				ReprotError(L"û�����ñ��⣬�޷������л���", L"CStdioDB::ConnectDatabase", __FILE__,__LINE__, NULL);
				m_spAdoCon.Release();
				return E_FAIL;
			}
		}
		else
		{
			RegGetString(_T("ServerName"), _T("cctvdb"), strServerName, bIsBackupDatabase,TRUE);
		}
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
				//_T("Driver={MySQL ODBC 5.1 Driver};server=%s;database=%s;OPTION=3"),strServerName,strDatabaseName);
				_T("Driver={MySQL ODBC 5.1 Driver};server=%s;database=%s;port=%s;OPTION=3"),strServerName,strDatabaseName,strPort);
			m_lDatabaseType = DB_MYSQL;
			//ReprotError(L"���ݿ����ͣ�Mysql��", L"CDBInterface::ConnectDatabase", __FILE__,__LINE__, NULL);
		}
		else
		{
			m_lDatabaseType = 0;
			ReprotError(L"���õ����ݿ����ͷǷ���", L"CStdioDB::ConnectDatabase", __FILE__,__LINE__, NULL);
			m_spAdoCon.Release();
			return E_FAIL;
		}

		if(m_lDatabaseType == DB_ORACLE)
		{
			// ʹ�ô�ע����ж�ȡ�Ĺ������ַ����������ݿ�
			//hr = m_spAdoCon->Open(strConnectionString,strUserID,m_bstrConstr,adAsyncConnect);
			hr = m_spAdoCon->Open(strConnectionString,strUserID,strConstructString,adAsyncConnect);

			if (FAILED(hr))
			{
				TCHAR lpszBuffer [256];
				_stprintf(lpszBuffer,_T("�������ݿ�ʧ��,[%x]"), hr);
				ReprotError(lpszBuffer, L"CStdioDB::ConnectDatabase", __FILE__,__LINE__, NULL);

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
			if (lState != adStateOpen)
			{
				TCHAR lpszBuffer [256];
				_stprintf(lpszBuffer,_T("�������ݿ�Ĳ�������ȷʧ�ܣ�,[%d]"), lState);
				ReprotError(lpszBuffer, L"CStdioDB::ConnectDatabase", __FILE__,__LINE__, NULL);

				m_spAdoCon.Release();

				return E_FAIL;
			}
		}
		else
		{
			// ʹ�ô�ע����ж�ȡ�Ĺ������ַ����������ݿ�
			hr = m_spAdoCon->Open(strConnectionString,strUserID,strConstructString,adConnectUnspecified);
			if(FAILED(hr))
			{
				ReprotError(L"�������ݿ�ʧ�ܣ�", L"CStdioDB::ConnectDatabase", __FILE__,__LINE__, NULL);
				m_spAdoCon.Release();

				return hr;
			}
		}
		ReprotError(L"�������ݿ�ɹ���", L"CStdioDB::ConnectDatabase", __FILE__,__LINE__, NULL);
		//������Ѿ�ִ���˿�ʼ����ģ��������Ӻ�Ҳ���뿪������
		if(m_bBeginTrans)
		{
			m_spAdoCon->BeginTrans ();
		}
	}
	catch(_com_error e)
	{
		m_spAdoCon.Release();
		return ReprotError(&e, L"CStdioDB::ConnectDatabase", __FILE__,__LINE__);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::ConnectDatabase",__FILE__,__LINE__,NULL);
		return E_FAIL;
	}

	m_bIsBackupDatabase = bIsBackupDatabase;
	return S_OK;
}

HRESULT CStdioDB::ReprotError(LPWSTR lpswError, LPWSTR lpswErrorMothod, LPCSTR lpszFileName, long lLineNo, LPWSTR lpswRemark)
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
		hr = WriteLog(bstrErrorDescription, bstrComputer, m_iStdioDBID, bstrErrorMothod, bstrFileName, lLineNo, bstrAuthorName, bstrRemark);
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

//ȡ��һ��SQL����ȡ�ս������SQL���
BOOL CStdioDB::GetRelationSql(BSTR bstrSql,LPTSTR lpszBuf)
{
	BOOL bSqlSelect;
	long i,lCount,lMax,lWherePos = 0,lGroupPos = 0,lOrderPos = 0,lUnionPos = 0,lGroupLength = 0,lLevel = 0;
	lCount = ::SysStringLen(bstrSql);
	memset(lpszBuf,0,lCount + WHERE_LENGTH);
	_tcscpy(lpszBuf,bstrSql);
	_tcsupr(lpszBuf);
	for(i = 0;i < lCount;i++)
	{
		bSqlSelect = (lWherePos == 0&&lGroupPos == 0&&lOrderPos == 0&&lUnionPos == 0&&lGroupLength == 0&&lLevel == 0);
		switch(lpszBuf[i])
		{
		case _T('+'):
		case _T('-'):
		case _T('/'):
			if(bSqlSelect)
			{
				return FALSE;
			}
			break;
		case _T('|'):
			if(bSqlSelect)
			{
				if(lpszBuf[i + 1] == _T('|'))
					return FALSE;
			}
			break;
		case _T('A'):
			if(bSqlSelect)
			{
				if(_tcsncmp(lpszBuf + i,_T("AVG"),3) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 3] < 48)
						return FALSE;
				}
			}
			break;
		case _T('C'):
			if(bSqlSelect)
			{
				if(_tcsncmp(lpszBuf + i,_T("COUNT"),5) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 5] < 48)
						return FALSE;
				}
			}
			break;
		case _T('D'):
			if(bSqlSelect)
			{
				if(_tcsncmp(lpszBuf + i,_T("DISTINCT"),8) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 8] < 48)
						return FALSE;
				}
				if(_tcsncmp(lpszBuf + i,_T("DECODE"),6) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 6] < 48)
						return FALSE;
				}
			}
			break;
		case _T('L'):
			if(bSqlSelect)
			{
				if(_tcsncmp(lpszBuf + i,_T("LOWER"),5) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 5] < 48)
						return FALSE;
				}
			}
			break;
		case _T('M'):
			if(bSqlSelect)
			{
				if(_tcsncmp(lpszBuf + i,_T("MAX"),3) == 0)
				{
					if(lpszBuf[i - 1] <= 48&&lpszBuf[i + 3] <= 48)
						return FALSE;
				}
				else if(_tcsncmp(lpszBuf + i,_T("MIN"),3) == 0)
				{
					if(lpszBuf[i - 1] <= 48&&lpszBuf[i + 3] <= 48)
						return FALSE;
				}
			}
			break;
		case _T('N'):
			if(bSqlSelect)
			{
				if(_tcsncmp(lpszBuf + i,_T("NVL"),3) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 3] < 48)
						return FALSE;
				}
				if(_tcsncmp(lpszBuf + i,_T("NVL2"),4) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 4] < 48)
						return FALSE;
				}
			}
			break;
		case _T('S'):
			if(bSqlSelect)
			{
				if(_tcsncmp(lpszBuf + i,_T("SUM"),3) == 0)
				{
					if(lpszBuf[i - 1] <= 48&&lpszBuf[i + 3] <= 48)
						return FALSE;
				}
				else if(_tcsncmp(lpszBuf + i,_T("SYSDATE"),7) == 0)
				{
					if(lpszBuf[i - 1] <= 48&&lpszBuf[i + 7] <= 48)
						return FALSE;
				}
			}
			break;
		case _T('T'):
			if(bSqlSelect)
			{
				if(_tcsncmp(lpszBuf + i,_T("TRIM"),4) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 4] < 48)
						return FALSE;
				}
				else if(_tcsncmp(lpszBuf + i,_T("TO_DATE"),7) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 7] < 48)
						return FALSE;
				}
				else if(_tcsncmp(lpszBuf + i,_T("TO_CHAR"),7) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 7] < 48)
						return FALSE;
				}
			}
			break;
		case _T('W'):
			if(lWherePos == 0&&lLevel == 0)
			{
				if(_tcsncmp(lpszBuf + i,_T("WHERE"),5) == 0)
				{
					if((lpszBuf[i-1] > 0 && lpszBuf[i-1] < _T('0'))&&(lpszBuf[i+5] > 0 && lpszBuf[i+5] < _T('0')))
					{
						lWherePos = i;
						if(lGroupPos > 0&&lGroupLength == 0&&lUnionPos == 0)
							lGroupLength = i - lGroupPos -1;
						i = i + 5;
					}
				}
			}
			break;
		case _T('G'):
			if(lGroupPos == 0&&lLevel == 0)
			{
				if(_tcsncmp(lpszBuf + i,_T("GROUP"),5) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 5] < 48)
						return FALSE;
				}
			}
			break;
		case _T('O'):
			if(lOrderPos == 0&&lLevel == 0)
			{
				if(_tcsncmp(lpszBuf + i,_T("ORDER"),5) == 0)
				{
					if((lpszBuf[i-1] > 0 && lpszBuf[i-1] < _T('0'))&&(lpszBuf[i+5] > 0 && lpszBuf[i+5] < _T('0')))
					{
						lOrderPos = i;
						if(lGroupPos > 0&&lGroupLength == 0&&lUnionPos == 0)
							lGroupLength = i - lGroupPos -1;
						i = i + 5;
					}
				}
			}
			break;
		case _T('U'):
			if(bSqlSelect)
			{
				if(_tcsncmp(lpszBuf + i,_T("UPPER"),5) == 0)
				{
					if(lpszBuf[i - 1] <= 48&&lpszBuf[i + 5] <= 48)
						return FALSE;
				}
			}
			if(lUnionPos == 0&&lLevel == 0)
			{
				if(_tcsncmp(lpszBuf + i,_T("UNION"),5) == 0)
				{
					if((lpszBuf[i-1] > 0 && lpszBuf[i-1] < _T('0'))&&(lpszBuf[i+5] > 0 && lpszBuf[i+5] < _T('0')))
					{
						lUnionPos = i;
						if(lGroupPos > 0&&lGroupLength == 0&&lUnionPos == 0)
							lGroupLength = i - lGroupPos -1;
						i = i + 5;
					}
				}
			}
			break;
		case _T('H'):
			if(lLevel == 0)
			{
				if(_tcsncmp(lpszBuf + i,_T("HAVING"),6) == 0)
				{
					if(lpszBuf[i - 1] < 48&&lpszBuf[i + 6] < 48)
						return FALSE;
				}
			}
			break;
		case _T('('):
			if(bSqlSelect)
			{
				return FALSE;
			}
			lLevel++;
			break;
		case _T(')'):
			if(bSqlSelect)
			{
				return FALSE;
			}
			lLevel--;
			break;
		}
	}
	memset(lpszBuf,0,lCount + WHERE_LENGTH);
	if(lUnionPos > 0)
		lCount = lUnionPos - 1;

	if(lWherePos > 0)
	{
		lMax = lWherePos;
	}
	else
	{
		lMax = lOrderPos;
		if(lMax > lUnionPos)
			lMax = lUnionPos;
	}
	if(lMax == 0)
	{
		lMax = lCount;
	}
	_tcsncpy(lpszBuf,bstrSql,lMax);
	_tcscpy(lpszBuf + lMax,_T(" WHERE 1 = 2 "));
	if(lGroupPos > 0)
		return FALSE;
	else
		return TRUE;
}

HRESULT CStdioDB::CopyRecordset(_Recordset *pSrcRs, _Recordset *pTarRs,LPCTSTR lpszOldSql,LPCTSTR lpszNewSql)
{
	HRESULT hr;
	_variant_t var;
	_bstr_t bstrFieldName;
	FieldsPtr pFields,pNewFields;
	long lCountField,i,lStateStatus,lRow;
	lRow = 0;
	try
	{
		if(!pSrcRs) return S_OK;
		pSrcRs->get_State(&lStateStatus);
		if(lStateStatus != adStateOpen) return S_OK;
		if(pSrcRs->adoEOF) return S_OK;

		pFields = pSrcRs->Fields;
		pNewFields = pTarRs->Fields;
		lCountField = pSrcRs->Fields->GetCount();
		while(!pSrcRs->adoEOF)
		{
			hr = pTarRs->AddNew();
			if(FAILED(hr)) return hr;
			for(i = 0;i < lCountField;i++)
			{
				var = pFields->GetItem(i)->Value;
				if(var.vt != VT_NULL&&var.vt != VT_EMPTY&&var.vt != VT_ERROR)
				{
					bstrFieldName = pFields->GetItem(i)->Name;
					pNewFields->GetItem(bstrFieldName)->Value = var;
					var.Clear();
				}
			}
			pTarRs->UpdateBatch(adAffectCurrent);
			pSrcRs->MoveNext();
			lRow++;
		}
		pFields.Release();
		pNewFields.Release();
		pTarRs->MoveFirst();
	}
	catch(_com_error e)
	{
		TCHAR lpszBuf[40960];
		_stprintf(lpszBuf,_T("ԭSQL��䣺%s����SQL��䣺%s����ǰ����������%d"),lpszOldSql,lpszNewSql,lRow);
		return ReprotError(&e, L"CStdioDB::CopyRecordset", __FILE__,__LINE__,lpszBuf);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::CopyRecordset",__FILE__,__LINE__);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CStdioDB::GetRecordsetWithLOB(BSTR bstrSql, _Recordset **ppRs)
{
	BOOL bHaveLOB = FALSE;
	if(m_lDBRunMode&&m_lDatabaseType == DB_ORACLE)
	{
		bHaveLOB = CheckHaveLOB(bstrSql);
	}

	if((m_lDBRunMode == 0||m_lDatabaseType != DB_ORACLE)||(m_lDBRunMode&&m_lDatabaseType == DB_ORACLE&&!bHaveLOB))
	{
		HRESULT hr;
		BOOL bCreateRs;
		long lCount = 0;
		_RecordsetPtr spAdoRs;
REGETRECORDSETLOB:
		bCreateRs = FALSE;
		lCount++;
		if(lCount > 3)
		{
			ReprotError(L"���ݿ������⣬�������ݿ⣡", L"CStdioDB::GetRecordsetWithLOB",__FILE__,__LINE__,bstrSql);
			return E_FAIL;
		}

		// ������������������ݿ��ָ��
		hr = PrepareConnectPtr();
		if(FAILED(hr)) return hr;

		try
		{
			hr = spAdoRs.CreateInstance(__uuidof(Recordset));
			if(FAILED(hr)) return hr;
			bCreateRs = TRUE;
			spAdoRs->PutCursorLocation(adUseClientBatch);
			spAdoRs->Open(bstrSql, m_spAdoCon.GetInterfacePtr(), adOpenStatic, adLockBatchOptimistic, -1);
			VARIANT var;
			var.vt = VT_DISPATCH;
			var.pdispVal = NULL;
			spAdoRs->PutActiveConnection(var);
			*ppRs = spAdoRs.Detach();
		}
		catch(_com_error e)
		{
			if(FAILED(ReConnect(&e)))
				return ReprotError(&e, L"CStdioDB::GetRecordsetWithLOB",__FILE__,__LINE__,bstrSql);
			else
			{
				if(bCreateRs)
					spAdoRs.Release();
				goto REGETRECORDSETLOB;
			}
		}
		catch(...)
		{
			ReprotError(_T("������ͨ�쳣"), L"CStdioDB::GetRecordsetWithLOB",__FILE__,__LINE__,bstrSql);
			return E_FAIL;
		}
	}
	else
	{
		HRESULT hr;
		VARIANT var;
		BOOL bHaveLobField;
		long lCount = 0;
		LPTSTR lpszBuf = NULL;
		_CommandPtr spAdoCmd = NULL;
		_ParameterPtr spAdoClob = NULL;
		_RecordsetPtr spAdoRs = NULL,spCpRs = NULL;
		VariantInit(&var);
REGETRECORDSETWITHLOB:
		lCount++;
		if(lCount > 3)
		{
			ReprotError(L"���ݿ������⣬�������ݿ⣡", L"CStdioDB::GetRecordsetWithLOB",__FILE__,__LINE__,bstrSql);
			return E_FAIL;
		}

		// ������������������ݿ��ָ��
		hr = PrepareConnectPtr();
		if(FAILED(hr)) return hr;

		try
		{
			bHaveLobField = TRUE;
			lpszBuf = new TCHAR[::SysStringLen(bstrSql) + WHERE_LENGTH];
			if(GetRelationSql(bstrSql,lpszBuf))
			{
				hr = spAdoRs.CreateInstance(__uuidof(Recordset));
				if(FAILED(hr)) return hr;
				spAdoRs->PutCursorLocation(adUseClientBatch);
				spAdoRs->Open(lpszBuf, m_spAdoCon.GetInterfacePtr(), adOpenStatic, adLockBatchOptimistic, -1);

				var.vt = VT_DISPATCH;
				var.pdispVal = NULL;
				spAdoRs->PutActiveConnection(var);
				bHaveLobField = CheckHaveLOBField(spAdoRs);
				if(bHaveLobField)
				{
					VariantInit(&var);
					//�µķ������Խ��CLOB�ڴ�й©������
					m_spAdoCon->PutCursorLocation(adUseClientBatch);
					hr = spAdoCmd.CreateInstance(__uuidof(Command));
					if(FAILED(hr)) return hr;
					spAdoCmd->ActiveConnection = m_spAdoCon;
					spAdoClob = spAdoCmd->CreateParameter(L"prCLOB",adLongVarChar,adParamOutput,100000);
					hr = spAdoCmd->Parameters->Append(spAdoClob);
					spAdoCmd->Properties->GetItem(L"SPPrmsLOB")->Value = 1L;

					spAdoCmd->CommandText = bstrSql;
					spAdoCmd->CommandType = adCmdText;
					spCpRs = spAdoCmd->Execute(&var,NULL,adCmdText);
					m_spAdoCon->PutCursorLocation(adUseServer);

					if(spAdoRs)
					{
						CopyRecordset(spCpRs, spAdoRs,bstrSql,lpszBuf);
						*ppRs = spAdoRs.Detach();
					}
					else
					{
						var.vt = VT_DISPATCH;
						var.pdispVal = NULL;
						spCpRs->PutActiveConnection(var);
						*ppRs = spCpRs.Detach();
					}
				}
				else
				{
					hr = spAdoRs.CreateInstance(__uuidof(Recordset));
					if(FAILED(hr)) return hr;
					spAdoRs->PutCursorLocation(adUseClientBatch);
					spAdoRs->Open(bstrSql, m_spAdoCon.GetInterfacePtr(), adOpenStatic, adLockBatchOptimistic, -1);
					var.vt = VT_DISPATCH;
					var.pdispVal = NULL;
					spAdoRs->PutActiveConnection(var);
					*ppRs = spAdoRs.Detach();
				}
			}
			else
			{
				VariantInit(&var);
				//�µķ������Խ��CLOB�ڴ�й©������
				m_spAdoCon->PutCursorLocation(adUseClientBatch);
				hr = spAdoCmd.CreateInstance(__uuidof(Command));
				if(FAILED(hr)) return hr;
				spAdoCmd->ActiveConnection = m_spAdoCon;
				spAdoClob = spAdoCmd->CreateParameter(L"prCLOB",adLongVarChar,adParamOutput,100000);
				hr = spAdoCmd->Parameters->Append(spAdoClob);
				spAdoCmd->Properties->GetItem(L"SPPrmsLOB")->Value = 1L;

				spAdoCmd->CommandText = bstrSql;
				spAdoCmd->CommandType = adCmdText;
				spCpRs = spAdoCmd->Execute(&var,NULL,adCmdText);
				m_spAdoCon->PutCursorLocation(adUseServer);
				*ppRs = spCpRs.Detach();
			}
			VariantClear(&var);
		}
		catch(_com_error e)
		{
			m_spAdoCon->PutCursorLocation(adUseServer);
			if(lpszBuf)
			{
				ReprotError(&e, L"CStdioDB::GetRecordsetWithLOB",__FILE__,__LINE__,CComBSTR(lpszBuf));
				delete [] lpszBuf;
				lpszBuf = NULL;
			}
			if(spAdoCmd)	spAdoCmd.Release();
			if(spAdoClob)	spAdoClob.Release();
			if(spAdoRs)		spAdoRs.Release();
			if(spCpRs)		spAdoRs.Release();
			VariantClear(&var);
			if(FAILED(ReConnect(&e)))
			{
				ReprotError(&e, L"CStdioDB::GetRecordsetWithLOB",__FILE__,__LINE__,bstrSql);
				return E_FAIL;
			}
			else
			{
				goto REGETRECORDSETWITHLOB;
			}
		}
		catch(...)
		{
			if(spAdoCmd)	spAdoCmd.Release();
			if(spAdoClob)	spAdoClob.Release();
			if(spCpRs)
			{
				long lState;
				spCpRs->get_State(&lState);
				if(lState != adStateClosed)
				{
					spCpRs->Close();
				}
				spCpRs.Release();
			}
			if(lpszBuf)
			{
				delete [] lpszBuf;
				lpszBuf = NULL;
			}
			ReprotError(_T("������ͨ�쳣"), L"CStdioDB::GetRecordsetWithLOB",__FILE__,__LINE__,bstrSql);
			return E_FAIL;
		}

		if(spAdoCmd)	spAdoCmd.Release();
		if(spAdoClob)	spAdoClob.Release();
		if(spCpRs)
		{
			long lState;
			spCpRs->get_State(&lState);
			if(lState != adStateClosed)
			{
				spCpRs->Close();
			}
			spCpRs.Release();
		}
		if(lpszBuf)
		{
			delete [] lpszBuf;
			lpszBuf = NULL;
		}
	}
	return S_OK;
}

STDMETHODIMP CStdioDB::UpdateRecordsetWithLOB(_Recordset *pRs)
{
	HRESULT hr;
	VARIANT varCon;
	_variant_t var,var1;
	TCHAR lpszSql[350];
	BOOL bSinglePK = TRUE,bUpdate;
	_RecordsetPtr pKeyColRs,pNewRs,pLobRs;
	long lStateStatus,lCount,i;
	_bstr_t bstrTableName,bstrColName,bstrFields,bstrSql,bstrWhere,bstrPkFielName,bstrTemp;
	VariantInit(&varCon);
	try
	{
		// ������������������ݿ��ָ��
		hr = PrepareConnectPtr();
		if(FAILED(hr)) return hr;

		if(m_lDatabaseType != DB_ORACLE||(!m_lDBRunMode&&m_lDatabaseType == DB_ORACLE))
		{
			pRs->PutActiveConnection(m_spAdoCon.GetInterfacePtr());
			pRs->UpdateBatch(adAffectAll);

			varCon.vt = VT_DISPATCH;
			varCon.pdispVal = NULL;
			pRs->PutActiveConnection(varCon);
			VariantClear(&varCon);
		}
		else
		{
			if(!pRs) return S_OK;
			pRs->get_State(&lStateStatus);
			if(lStateStatus != adStateOpen) return S_OK;
			if(pRs->adoEOF) return S_OK;
			bstrTableName = pRs->Fields->GetItem(0L)->Properties->GetItem(L"BASETABLENAME")->Value;
			memset(lpszSql,0,sizeof(TCHAR)*350);
			_stprintf(lpszSql, _T("SELECT NAME FROM SYS.COL$ WHERE (TYPE# = 112 OR TYPE# = 113) AND OBJ# IN (SELECT OBJ# FROM SYS.OBJ$ WHERE NAME = '%s' AND OWNER# = USERENV('SCHEMAID'))"),(LPCTSTR)bstrTableName);
			hr = pLobRs.CreateInstance(__uuidof(Recordset));
			if(FAILED(hr)) return hr;
			pLobRs->PutCursorLocation(adUseClientBatch);
			hr = pLobRs->Open(lpszSql, m_spAdoCon.GetInterfacePtr() , adOpenStatic, adLockBatchOptimistic, adCmdText);
			if(FAILED(hr)) return hr;
			varCon.vt = VT_DISPATCH;
			varCon.pdispVal = NULL;
			pLobRs->PutActiveConnection(varCon);

			memset(lpszSql,0,sizeof(TCHAR)*350);
			_stprintf(lpszSql, _T("SELECT COL.NAME as COLUMN_NAME FROM  SYS.COL$ COL,SYS.CCOL$ CC,SYS.CDEF$ CD,SYS.OBJ$ O WHERE CD.TYPE# = 2 AND CD.CON# = CC.CON# AND CC.OBJ# = COL.OBJ# AND CC.INTCOL# = COL.INTCOL# AND CD.OBJ# = O.OBJ# AND O.OWNER# = USERENV('SCHEMAID') AND O.NAME = '%s'"),(LPCTSTR)bstrTableName);
			hr = pKeyColRs.CreateInstance(__uuidof(Recordset));
			if(FAILED(hr)) return hr;
			pKeyColRs->PutCursorLocation(adUseClientBatch);
			hr = pKeyColRs->Open(lpszSql, m_spAdoCon.GetInterfacePtr() , adOpenStatic, adLockReadOnly, adCmdText);
			if(FAILED(hr)) return hr;
			varCon.vt = VT_DISPATCH;
			varCon.pdispVal = NULL;
			pKeyColRs->PutActiveConnection(varCon);
			VariantClear(&varCon);

			lCount = pRs->Fields->GetCount();
			for(;!pLobRs->adoEOF;pLobRs->MoveNext())
			{
				BOOL bIsLob = FALSE;
				var = pLobRs->Fields->GetItem(L"NAME")->Value;
				var.ChangeType(VT_BSTR);
				bstrTemp = var.bstrVal;
				var.Clear();
				for(i = 0;i < lCount;i++)
				{
					bstrColName = pRs->Fields->GetItem(i)->Properties->GetItem(L"BASECOLUMNNAME")->Value;
					if(wcscmp(bstrTemp,bstrColName) == 0)
					{
						bIsLob = TRUE;
						break;
					}
				}
				if(!bIsLob)
				{
					pLobRs->Delete(adAffectCurrent);
					pLobRs->UpdateBatch(adAffectCurrent);
				}
			}

			if(pLobRs->GetRecordCount() == 0)
			{
				bstrFields = pRs->Fields->GetItem(0L)->Properties->GetItem(L"BASECOLUMNNAME")->Value;
				for(i = 1;i < lCount;i++)
				{
					bstrColName = pRs->Fields->GetItem(i)->Properties->GetItem(L"BASECOLUMNNAME")->Value;
					bstrFields += L"," + bstrColName;
				}
			}
			else if(pLobRs->GetRecordCount() == 1)
			{
				_bstr_t bstrLobField;
				pLobRs->MoveFirst();
				var = pLobRs->Fields->GetItem(L"NAME")->Value;
				var.ChangeType(VT_BSTR);
				bstrLobField = var.bstrVal;
				bstrFields = L"";
				for(i = 0;i < lCount;i++)
				{
					bstrColName = pRs->Fields->GetItem(i)->Properties->GetItem(L"BASECOLUMNNAME")->Value;
					if(wcscmp(bstrLobField,bstrColName) == 0) continue;
					if(bstrFields.length() > 0)
						bstrFields += L",";
					bstrFields += bstrColName;
				}
			}
			else
			{
				BOOL bIsLob;
				bstrFields = L"";
				for(i = 0;i < lCount;i++)
				{
					bIsLob = FALSE;
					bstrColName = pRs->Fields->GetItem(i)->Properties->GetItem(L"BASECOLUMNNAME")->Value;
					for(pLobRs->MoveFirst();!pLobRs->adoEOF;pLobRs->MoveNext())
					{
						var = pLobRs->Fields->GetItem(L"NAME")->Value;
						var.ChangeType(VT_BSTR);
						if(wcscmp(var.bstrVal,bstrColName) == 0)
						{
							var.Clear();
							bIsLob = TRUE;
							break;
						}
						var.Clear();
					}
					if(bIsLob) continue;
					if(bstrFields.length() > 0)
						bstrFields += L",";
					bstrFields += bstrColName;
				}
			}

			lCount = pKeyColRs->GetRecordCount();
			if(lCount == 0)//û�����޷�����
				return E_FAIL;
			else if(lCount == 1)
			{
				bSinglePK = TRUE;
				BOOL bMuiltValue = FALSE;
				var = pKeyColRs->Fields->GetItem(L"COLUMN_NAME")->Value;
				var.ChangeType(VT_BSTR);
				bstrPkFielName = var.bstrVal;
				var.Clear();
				bstrWhere = L"";
				for(pRs->MoveFirst();!pRs->adoEOF;pRs->MoveNext())
				{
					if(pRs->Status != adRecModified)
						continue;
					var = pRs->Fields->GetItem(bstrPkFielName)->OriginalValue;
					switch(var.vt)
					{
					case VT_I1:
					case VT_I2:
					case VT_I4:
					case VT_I8:
					case VT_INT:
					case VT_UI1:
					case VT_UI2:
					case VT_UI4:
					case VT_UI8:
					case VT_UINT:
					case VT_R4:
					case VT_R8:
					case VT_DECIMAL:
						var.ChangeType(VT_BSTR);
						if(bstrWhere.length() > 0)
						{
							bMuiltValue = TRUE;
							bstrWhere += L",";
						}
						bstrWhere += var.bstrVal;
						var.Clear();
						break;
					case VT_DATE://������ ���ڲ�֧����������
						break;
					case VT_LPSTR://�ַ�����
					case VT_LPWSTR:
					case VT_BSTR:
						var.ChangeType(VT_BSTR);
						if(bstrWhere.length() > 0)
						{
							bMuiltValue = TRUE;
							bstrWhere += L",";
						}
						bstrWhere += L"'";
						bstrWhere += var.bstrVal;
						bstrWhere += L"'";
						var.Clear();
						break;
					}
				}
				if(bstrWhere.length() > 0)
				{
					if(bMuiltValue)
					{
						bstrWhere = bstrPkFielName + L" IN (" + bstrWhere + L")";
					}
					else
					{
						bstrWhere = bstrPkFielName + L" = " + bstrWhere;
					}
				}
				else
					bstrWhere = L"1 = 2";
			}
			else
			{
				_bstr_t bstrTemp;
				bSinglePK = FALSE;
				bstrWhere = L"";
				for(pRs->MoveFirst();!pRs->adoEOF;pRs->MoveNext())
				{
					if(pRs->Status != adRecModified)
						continue;
					bstrTemp = L"";
					for(pKeyColRs->MoveFirst();!pKeyColRs->adoEOF;pKeyColRs->MoveNext())
					{
						var = pKeyColRs->Fields->GetItem(L"COLUMN_NAME")->Value;
						var.ChangeType(VT_BSTR);
						bstrColName = var.bstrVal;
						var.Clear();
						var = pRs->Fields->GetItem(bstrColName)->OriginalValue;
						switch(var.vt)
						{
						case VT_I1:
						case VT_I2:
						case VT_I4:
						case VT_I8:
						case VT_INT:
						case VT_UI1:
						case VT_UI2:
						case VT_UI4:
						case VT_UI8:
						case VT_UINT:
						case VT_R4:
						case VT_R8:
						case VT_DECIMAL:
							var.ChangeType(VT_BSTR);
							if(bstrTemp.length() > 0)
							{
								bstrTemp += L" AND ";
							}
							bstrTemp += bstrColName + L" = " + var.bstrVal;
							var.Clear();
							break;
						case VT_DATE://������ ���ڲ�֧����������
							break;
						case VT_LPSTR://�ַ�����
						case VT_LPWSTR:
						case VT_BSTR:
							var.ChangeType(VT_BSTR);
							if(bstrTemp.length() > 0)
							{
								bstrTemp += L" AND ";
							}
							bstrTemp += bstrColName + L" = '" + var.bstrVal + L"'";
							var.Clear();
							break;
						}
					}
					if(bstrTemp.length() > 0)
					{
						bstrTemp = L"(" + bstrTemp + L")";
						if(bstrWhere.length() > 0)
							bstrWhere += L" OR ";
						bstrWhere += bstrTemp;
					}
				}
				if(bstrWhere.length() > 0)
				{
					bstrWhere = L"(" + bstrWhere + L")";
				}
				else
					bstrWhere = L"1 = 2";
			}
			bstrSql = L"SELECT " + bstrFields + L" FROM " + bstrTableName + L" WHERE " + bstrWhere;
			hr = pNewRs.CreateInstance(__uuidof(Recordset));
			if(FAILED(hr)) return hr;
			//�����LOB�ֶ����������
			bUpdate = FALSE;
			pNewRs->PutCursorLocation(adUseClientBatch);
			hr = pNewRs->Open(bstrSql, m_spAdoCon.GetInterfacePtr() , adOpenStatic, adLockBatchOptimistic, adCmdText);
			lCount = pNewRs->Fields->GetCount();
			for(;!pNewRs->adoEOF;pNewRs->MoveNext())
			{
				if(bSinglePK)//�����������
				{
					var = pNewRs->Fields->GetItem(bstrPkFielName)->Value;
					var.ChangeType(VT_BSTR);
					for(pRs->MoveFirst();!pRs->adoEOF;pRs->MoveNext())
					{
						if(pRs->Status != adRecModified)
							continue;
						var1 = pRs->Fields->GetItem(bstrPkFielName)->OriginalValue;
						var1.ChangeType(VT_BSTR);
						if(wcscmp(var.bstrVal,var1.bstrVal) == 0)
						{
							var1.Clear();
							for(i = 0;i < lCount;i++)
							{
								bstrColName = pNewRs->Fields->GetItem(i)->Name;
								var = pRs->Fields->GetItem(bstrColName)->Value;
								var1 = pNewRs->Fields->GetItem(i)->Value;
								if(var != var1)
								{
									pNewRs->Fields->GetItem(i)->Value = var;
									bUpdate = TRUE;
								}
							}
							break;
						}
						var1.Clear();
					}
					var.Clear();
				}
				else
				{
					BOOL bFinded;
					for(pRs->MoveFirst();!pRs->adoEOF;pRs->MoveNext())
					{
						if(pRs->Status != adRecModified)
							continue;
						bFinded = TRUE;
						for(pKeyColRs->MoveFirst();!pKeyColRs->adoEOF;pKeyColRs->MoveNext())
						{
							var = pKeyColRs->Fields->GetItem(L"COLUMN_NAME")->Value;
							var.ChangeType(VT_BSTR);
							bstrColName = var.bstrVal;
							var = pNewRs->Fields->GetItem(bstrColName)->Value;
							var.ChangeType(VT_BSTR);

							var1 = pRs->Fields->GetItem(bstrColName)->OriginalValue;
							var1.ChangeType(VT_BSTR);
							if(wcscmp(var.bstrVal,var1.bstrVal) != 0)
							{
								var1.Clear();
								var.Clear();
								bFinded = FALSE;
								break;
							}
							var1.Clear();
							var.Clear();
						}
						if(bFinded)
						{
							for(i = 0;i < lCount;i++)
							{
								bstrColName = pNewRs->Fields->GetItem(i)->Name;
								var = pRs->Fields->GetItem(bstrColName)->Value;
								var1 = pNewRs->Fields->GetItem(i)->Value;
								if(var != var1)
								{
									pNewRs->Fields->GetItem(i)->Value = var;
									bUpdate = TRUE;
								}
							}
							break;
						}
					}
				}
			}
			if(pRs->GetRecordCount() > 0)
			{
				for(pRs->MoveFirst();!pRs->adoEOF;pRs->MoveNext())
				{
					if(pRs->Status != adRecNew)
						continue;

					hr = pNewRs->AddNew();
					if(FAILED(hr)) return hr;
					for(i = 0;i < lCount;i++)
					{
						bstrColName = pNewRs->Fields->GetItem(i)->Name;
						pNewRs->Fields->GetItem(i)->Value = pRs->Fields->GetItem(bstrColName)->Value;
					}
					bUpdate = TRUE;
				}
			}
			if(bUpdate)
			{
				hr = pNewRs->UpdateBatch(adAffectAll);
				if(FAILED(hr)) return hr;
			}

			if(pLobRs->GetRecordCount() == 0)
			{
				pRs->UpdateBatch(adAffectAll);
				pRs->MoveFirst();
				return S_OK;
			}

			long lLobSize;
			//�����LOB�ֶε�����
			for(pRs->MoveFirst();!pRs->adoEOF;pRs->MoveNext())
			{
				if(pRs->Status != adRecNew&&pRs->Status != adRecModified)
					continue;
				bstrWhere = L"";
				if(bSinglePK)
				{
					var = pRs->Fields->GetItem(bstrPkFielName)->Value;
					if(var.vt == VT_I1||var.vt == VT_I2||var.vt == VT_I4||var.vt == VT_I8||var.vt == VT_INT||
						var.vt == VT_UI1||var.vt == VT_UI2||var.vt == VT_UI4||var.vt == VT_UI8||var.vt == VT_UINT||
						var.vt == VT_R4||var.vt == VT_R8||var.vt == VT_DECIMAL)
					{
						var.ChangeType(VT_BSTR);
						bstrWhere = bstrPkFielName + L" = " + var.bstrVal;
					}
					else if(var.vt == VT_LPSTR||var.vt == VT_LPWSTR||var.vt == VT_BSTR)
					{
						var.ChangeType(VT_BSTR);
						bstrWhere = bstrPkFielName + L" = '" + var.bstrVal + L"'";
					}
					else if(var.vt == VT_DATE)//���ڲ�֧����������
					{
					}
				}
				else
				{
					for(pKeyColRs->MoveFirst();!pKeyColRs->adoEOF;pKeyColRs->MoveNext())
					{
						var = pKeyColRs->Fields->GetItem(L"COLUMN_NAME")->Value;
						var.ChangeType(VT_BSTR);
						bstrColName = var.bstrVal;
						var = pRs->Fields->GetItem(bstrColName)->Value;
						if(var.vt == VT_I1||var.vt == VT_I2||var.vt == VT_I4||var.vt == VT_I8||var.vt == VT_INT||
							var.vt == VT_UI1||var.vt == VT_UI2||var.vt == VT_UI4||var.vt == VT_UI8||var.vt == VT_UINT||
							var.vt == VT_R4||var.vt == VT_R8||var.vt == VT_DECIMAL)
						{
							var.ChangeType(VT_BSTR);
							if(bstrWhere.length() > 0)
							{
								bstrWhere += L" AND ";
							}
							bstrWhere += bstrColName + L" = " + var.bstrVal;
						}
						else if(var.vt == VT_LPSTR||var.vt == VT_LPWSTR||var.vt == VT_BSTR)
						{
							var.ChangeType(VT_BSTR);
							if(bstrWhere.length() > 0)
							{
								bstrWhere += L" AND ";
							}
							bstrWhere += bstrColName + L" = '" + var.bstrVal + L"'";
						}
						else if(var.vt == VT_DATE)//���ڲ�֧����������
						{
						}
					}
				}
				if(bstrWhere.length() == 0) continue;
				for(pLobRs->MoveFirst();!pLobRs->adoEOF;pLobRs->MoveNext())
				{
					var = pLobRs->Fields->GetItem(L"NAME")->Value;
					var.ChangeType(VT_BSTR);
					bstrColName = var.bstrVal;
					var.Clear();
					lLobSize = pRs->Fields->GetItem(bstrColName)->ActualSize;
					if(lLobSize == 0)
					{
						bstrSql = L"UPDATE " + bstrTableName + L" SET " + bstrColName + L" = NULL WHERE " + bstrWhere;
						m_spAdoCon->Execute(bstrSql,&var,adCmdText | adExecuteNoRecords);
					}
					else
					{
						pNewRs->Close();
						pNewRs->PutCursorLocation(adUseServer);
						bstrSql = L"SELECT " + bstrColName + L" FROM " + bstrTableName + L" WHERE " + bstrWhere;
						hr = pNewRs->Open(bstrSql, m_spAdoCon.GetInterfacePtr() , adOpenForwardOnly, adLockBatchOptimistic, adCmdText);
						if(pNewRs->adoEOF) break;//û�鵽���ݣ�һ���ǲ����ܵ�
						var = pRs->Fields->GetItem(bstrColName)->GetChunk(lLobSize);
						pNewRs->Fields->GetItem(bstrColName)->AppendChunk(var);
						pNewRs->UpdateBatch(adAffectCurrent);
						var.Clear();
					}
				}
				pRs->UpdateBatch(adAffectCurrent);
			}
		}
	}
	catch (_com_error e)
	{
		CComBSTR bstrRemark;
		bstrRemark = GetRemark(pRs);
		VariantClear(&varCon);
		if(e.Error() == 0x80040E38)
		{
			_variant_t varFilter;
			try
			{
				varFilter = (long)adFilterConflictingRecords;
				pRs->PutFilter(&varFilter);
				pRs->Resync(adAffectGroup, adResyncUnderlyingValues);
				pRs->UpdateBatch(adAffectGroup);
				varCon.vt = VT_DISPATCH;
				varCon.pdispVal = NULL;
				pRs->PutActiveConnection(varCon);
				VariantClear(&varCon);
			}
			catch(_com_error e1)
			{
				VariantClear(&varCon);
				ReprotError(&e, L"CStdioDB::UpdateRecordset",__FILE__,__LINE__,bstrRemark);
				return E_FAIL;
			}

			return S_OK;
		}

		varCon.vt = VT_DISPATCH;
		varCon.pdispVal = NULL;
		pRs->PutActiveConnection(varCon);
		VariantClear(&varCon);
		return ReprotError(&e, L"CStdioDB::UpdateRecordsetWithLOB",__FILE__,__LINE__,bstrRemark);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::UpdateRecordsetWithLOB",__FILE__,__LINE__,bstrSql);
		return E_FAIL;
	}
	if(pKeyColRs)
	{
		pKeyColRs->get_State(&lStateStatus);
		if(lStateStatus != adStateClosed)
		{
			pKeyColRs->Close();
		}
		pKeyColRs.Release();
	}
	if(pLobRs)
	{
		pLobRs->get_State(&lStateStatus);
		if(lStateStatus != adStateClosed)
		{
			pLobRs->Close();
		}
		pLobRs.Release();
	}
	if(pNewRs)
	{
		pNewRs->get_State(&lStateStatus);
		if(lStateStatus != adStateClosed)
		{
			pNewRs->Close();
		}
		pNewRs.Release();
	}

	return S_OK;
}

void CStdioDB::CheckLob(_Recordset *pRs)
{
	// ������������������ݿ��ָ��
	HRESULT hr = PrepareConnectPtr();
	if(FAILED(hr)) return ;

	VARIANT var;
	VariantInit(&var);
	try
	{
		_variant_t var;
		_RecordsetPtr pLobRs;
		long lStateStatus,lCount,i;
		TCHAR lpszSql[350],lpszBuf[500];
		_bstr_t bstrTableName,bstrColName,bstrSql,bstrTemp;
		if(!pRs) return;
		pRs->get_State(&lStateStatus);
		if(lStateStatus != adStateOpen) return;
		if(pRs->adoEOF) return ;
		lCount = pRs->Fields->GetCount();
		bstrTableName = L"";
		hr = pLobRs.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)) return;
		for(i = 0;i < lCount;i++)
		{
			var = pRs->Fields->GetItem(i)->Properties->GetItem(L"BASETABLENAME")->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY) continue;
			var.ChangeType(VT_BSTR);
			bstrTemp = var.bstrVal;
			if(bstrTemp != bstrTableName)
			{
				bstrTableName = bstrTemp;
				pLobRs->get_State(&lStateStatus);
				if(lStateStatus == adStateOpen)
				{
					pLobRs->Close();
				}
				memset(lpszSql,0,sizeof(TCHAR)*350);
				_stprintf(lpszSql, _T("SELECT NAME FROM SYS.COL$ WHERE (TYPE# = 112 OR TYPE# = 113) AND OBJ# = (SELECT OBJ# FROM SYS.OBJ$ WHERE NAME = '%s' AND OWNER# = USERENV('SCHEMAID'))"),(LPCTSTR)bstrTableName);
				pLobRs->PutCursorLocation(adUseClientBatch);
				hr = pLobRs->Open(lpszSql, m_spAdoCon.GetInterfacePtr() , adOpenStatic, adLockBatchOptimistic, adCmdText);
				if(FAILED(hr)) return;
			}
			if(pLobRs->GetRecordCount() == 0)
				continue;
			var = pRs->Fields->GetItem(i)->Properties->GetItem(L"BASECOLUMNNAME")->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY) continue;
			var.ChangeType(VT_BSTR);
			bstrColName = var.bstrVal;
			for(pLobRs->MoveFirst();!pLobRs->adoEOF;pLobRs->MoveNext())
			{
				var = pLobRs->Fields->GetItem(L"NAME")->Value;
				if(var.vt == VT_NULL||var.vt == VT_EMPTY) continue;
				var.ChangeType(VT_BSTR);
				if(wcscmp(var.bstrVal,bstrColName) == 0)
				{
					//д��LOB�ֶε���־��
					memset(lpszBuf,0,sizeof(TCHAR)*500);
					_stprintf(lpszBuf, _T("��Recordset�еı�%s��%s�ֶ���LOB��������������ڴ�й©��"),(LPCTSTR)bstrTableName,(LPCTSTR)bstrColName);
					ReprotError(lpszBuf, L"CStdioDB::CheckLob",__FILE__,__LINE__);
					break;
				}
			}
		}
	}
	catch (_com_error e)
	{
		ReprotError(&e, L"CStdioDB::CheckLob",__FILE__,__LINE__);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::CheckLob",__FILE__,__LINE__);
	}
	return;
}

STDMETHODIMP CStdioDB::LockItemData(BSTR bstrDataID, long lLockType, long lLoginInfoID, long lUserID, long *plLockFlag)
{
	_variant_t var;
	_variant_t varIn,varOut;
	_ParameterPtr spAdoParam1,spAdoParam2,spAdoParam3,spAdoParam4,spAdoParam5,spAdoParam6,spAdoParamOut;
	_CommandPtr spAdoCmd;
	HRESULT hr = 0;
	*plLockFlag = 0;
	try
	{
		// ������������������ݿ��ָ��
		hr = PrepareConnectPtr();
		if(FAILED(hr)) return hr;

		hr = spAdoCmd.CreateInstance(__uuidof(Command));
		if(FAILED(hr)) return hr;
		spAdoCmd->ActiveConnection = m_spAdoCon;
		spAdoCmd->CommandText = _T("DOC_LOCKDATA");
		spAdoCmd->CommandType = adCmdStoredProc;

		varIn.vt = VT_BSTR;
		varIn.bstrVal = bstrDataID;

		//db2��ʱ�����������ҪadWChar
		if(m_lDatabaseType==DB_DB2)
			spAdoParam1 = spAdoCmd->CreateParameter(_T("P_DATAID"),adWChar,adParamInput, ::SysStringLen(bstrDataID) * sizeof(TCHAR),varIn);
		else
			spAdoParam1 = spAdoCmd->CreateParameter(_T("P_DATAID"),adBSTR,adParamInput, ::SysStringLen(bstrDataID) * sizeof(TCHAR),varIn);
		spAdoCmd->Parameters->Append(spAdoParam1);
		spAdoParam1->Value = varIn;

		varIn.vt = VT_I4;
		varIn.lVal = lLockType;
		spAdoParam2 = spAdoCmd->CreateParameter(_T("P_LOCKTYPE"),adInteger,adParamInput, 4,varIn);
		spAdoCmd->Parameters->Append(spAdoParam2);
		spAdoParam2->Value = lLockType;

		varIn.vt = VT_I4;
		varIn.lVal = lLoginInfoID;
		spAdoParam3 = spAdoCmd->CreateParameter(_T("P_LOGININFOID"),adInteger,adParamInput, 4,varIn);
		spAdoCmd->Parameters->Append(spAdoParam3);
		spAdoParam3->Value = lLoginInfoID;

		varIn.vt = VT_I4;
		varIn.lVal = lUserID;
		spAdoParam4 = spAdoCmd->CreateParameter(_T("P_USERID"),adInteger,adParamInput, 4,varIn);
		spAdoCmd->Parameters->Append(spAdoParam4);
		spAdoParam4->Value = lUserID;

		varIn.vt = VT_I4;
		varIn.lVal = m_lWriteLogFlag;
		spAdoParam5 = spAdoCmd->CreateParameter(_T("P_WRITELOGFLAG"),adInteger,adParamInput, 4,varIn);
		spAdoCmd->Parameters->Append(spAdoParam5);
		spAdoParam5->Value = m_lWriteLogFlag;

		//db2��ʱ��������Ʊ仯��
		if(m_lDatabaseType==DB_DB2)
			spAdoParamOut = spAdoCmd->CreateParameter(_T("P_LOCKFLAG "),adInteger,adParamOutput,sizeof(int));
		else
			spAdoParamOut = spAdoCmd->CreateParameter(_T("RESULT"),adInteger,adParamOutput,sizeof(int));
		spAdoCmd->Parameters->Append(spAdoParamOut);
		spAdoCmd->Execute(NULL,NULL,adCmdStoredProc | adExecuteNoRecords); 
		varOut = spAdoParamOut->Value;
		if(varOut.vt != VT_EMPTY && varOut.vt != VT_NULL)
		{
			varOut.ChangeType(VT_I4);
			*plLockFlag = varOut.lVal;
		}
	}
	catch(_com_error e)
	{
		return ReprotError(&e, L"CStdioDB::LockItemData", __FILE__,__LINE__);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::LockItemData",__FILE__,__LINE__);
		return E_FAIL;
	}

	// һ��ִ�гɹ����ͽ�ȫ��ʱ�������Ϊ0
	g_dwFaildTime = 0;
	return S_OK;
}

STDMETHODIMP CStdioDB::CheckDBStatus(long *plDBStatus)
{
	HRESULT hr;
	long lState;
	long lCount;
	_variant_t var;
	_bstr_t bstrCmd;
	*plDBStatus = 1;
	try
	{
		hr = PrepareConnectPtr();
		if(FAILED(hr)) return hr;

		if(m_spAdoCon == NULL)
			return E_FAIL;
		m_spAdoCon->get_State(&lState);
		if(lState == adStateClosed)
		{
			return E_FAIL;
		}

		if(m_lDatabaseType == DB_ORACLE)
			bstrCmd = L"SELECT SYSDATE FROM DUAL";
		else if (m_lDatabaseType == DB_SQLSERVER || 
			m_lDatabaseType == DB_SYBASE)
			bstrCmd = L"select getdate()";
		else if (m_lDatabaseType == DB_DB2)
		{
			bstrCmd = L"select current timestamp from sysibm.sysdummy1";
		}
		m_spAdoCon->Execute(bstrCmd, &var, adCmdText | adExecuteNoRecords);
		lCount = (long)var;
		if(lCount == 0)
			return E_FAIL;
		*plDBStatus = 0;
	}
	catch(_com_error e)
	{
		return ReprotError(&e, L"CStdioDB::CheckDBStatus", __FILE__,__LINE__);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::CheckDBStatus",__FILE__,__LINE__);
		return E_FAIL;
	}
	// һ��ִ�гɹ����ͽ�ȫ��ʱ�������Ϊ0
	g_dwFaildTime = 0;
	return S_OK;
}

CComBSTR CStdioDB::GetRemark(_Recordset *pRs)
{
	CComBSTR bstrRet=_T("");
	try
	{
		long nCount;
		_bstr_t bstrTableName,bstrFieldName,bstrTemp;
		bstrTableName = pRs->Fields->GetItem(0L)->Properties->GetItem(L"BASETABLENAME")->Value;
		nCount = pRs->Fields->GetCount();
		for(long i= 0;i < nCount;i++)
		{
			bstrFieldName=pRs->Fields->GetItem(i)->Name;
			if(i>0)
			{
				bstrTemp=bstrTemp+_T(",")+bstrFieldName;
			}
			else
			{
				bstrTemp=_T(";���µ��ֶΣ�")+bstrFieldName;
			}			
		}
		_bstr_t bstr=_T("���µı�Ϊ��")+bstrTableName+bstrTemp;
		bstrRet=(BSTR)bstr;
	}
	catch (...)
	{
		return bstrRet;
	}
	return bstrRet;
}

BOOL CStdioDB::IsQuerySql(BSTR bstrSql)
{
	int lCount;
	TCHAR * pBuffer;
	pBuffer = bstrSql;
	lCount = ::SysStringLen(bstrSql);
	for(int i = 0;i < lCount;i++,pBuffer++)
	{
		if(pBuffer[0] == _T('S') || pBuffer[0] == _T('s'))
		{
			if((pBuffer[1] == _T('E')||pBuffer[1] == _T('e'))&&
				(pBuffer[2] == _T('L')||pBuffer[2] == _T('l'))&&
				(pBuffer[3] == _T('E')||pBuffer[3] == _T('e'))&&
				(pBuffer[4] == _T('C')||pBuffer[4] == _T('c'))&&
				(pBuffer[5] == _T('T')||pBuffer[5] == _T('t')))
				return TRUE;
		}
		else if(pBuffer[0] == _T('('))
		{
			continue;
		}
		else if(pBuffer[0] > _T(' '))
			return FALSE;
	}
	return FALSE;
}

// ������������������ݿ�
HRESULT CStdioDB::PrepareConnectPtr()
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
				ReprotError(L"lState == adStateClosed", L"CStdioDB::PrepareConnectPtr",__FILE__,__LINE__,L"������Ч�����жϣ�");
				m_spAdoCon.Release();
				bNeedReConnect = TRUE;
			}
		}
		else if (m_spAdoCon == NULL)
		{
			ReprotError(L"m_spAdoCon == NULL", L"CStdioDB::PrepareConnectPtr",__FILE__,__LINE__,L"������Ч�����жϣ�");
			bNeedReConnect = TRUE;
		}

		// ��������߳��Ѿ��е������ˣ����Լ�Ҳ�е�����
		BOOL bIsBackupDatebase = FALSE;
		if (g_bIsUserBackupDatebase == TRUE && m_bIsBackupDatabase == FALSE)
		{
			bNeedReConnect = TRUE;
			bIsBackupDatebase = TRUE;
		}

		if(!bNeedReConnect)
		{
			return S_OK;
		}
		else
		{
			ReprotError(L"��Ҫ���������ݿ�", L"CStdioDB::PrepareConnectPtr",__FILE__,__LINE__,L"������Ч�����жϣ�");
			if(bIsBackupDatebase)
				return ConnectDatabase(bIsBackupDatebase);
			else
			{
				HRESULT hr;
				hr = ConnectDatabase();

				//��Ҫִ��SQL��������ȷ�ж�
				if (SUCCEEDED(hr) && CheckStatus())
				{
					g_dwFaildTime = 0;
					return S_OK;
				}
				else
				{
					DWORD dwCurrentTime = timeGetTime();
					TCHAR strDelayTiem[20];
					RegGetString(_T("DelayTime"), _T("0"), strDelayTiem,FALSE,TRUE);
					long lDelayTime = _ttol(strDelayTiem);
					if (lDelayTime != 0 && g_dwFaildTime != 0 && abs(dwCurrentTime - g_dwFaildTime) > lDelayTime*1000) // �����жϳ����趨ʱ�䣬���Զ����ӱ������ݿ�
					{
						hr = ConnectDatabase(TRUE);
						if (SUCCEEDED(hr))
						{
							// �޸�����������ϢΪ����
							RegChangeString();
							g_bIsUserBackupDatebase = TRUE;
							m_bIsBackupDatabase = TRUE;
							ReprotError(L"�л����������ݿ�ɹ���", L"CStdioDB::PrepareConnectPtr", __FILE__,__LINE__, NULL);
							return S_OK;
						}
					}
					return E_FAIL;
				}
			}
		}
	}
	catch(_com_error e)
	{
		return ReprotError(&e, L"CStdioDB::PrepareConnectPtr", __FILE__,__LINE__);
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::PrepareConnectPtr",__FILE__,__LINE__);
		return E_FAIL;
	}
	return E_FAIL;
}

BOOL CStdioDB::CheckStatus()
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
			bstrCmd = L"select now()";

		m_spAdoCon->Execute(bstrCmd, &var, adCmdText | adExecuteNoRecords);
	}
	catch(_com_error e)
	{
		return FALSE;
	}
	catch(...)
	{
		ReprotError(_T("������ͨ�쳣"), L"CStdioDB::CheckStatus",__FILE__,__LINE__);
		return FALSE;
	}
	return TRUE;
}