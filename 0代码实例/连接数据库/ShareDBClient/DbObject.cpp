#include "StdAfx.h"
#include ".\dbobject.h"


DWORD CDbObject::m_dwLastErrorTime = 0;

#define CHECKERROR \
	DWORD dw = GetTickCount();\
	if(m_dwLastErrorTime>0 && dw-m_dwLastErrorTime<0) return FALSE;


CDbObject::CDbObject(void)
{
	m_hWaiteHandle = CreateEvent(NULL,NULL,0,NULL);
	m_nCmdType = 0;
	m_pRs = NULL;
	m_lpszSql = NULL;
	m_lAffectCount = 0;
	m_bRetValue = FALSE;

	m_bUse = TRUE;
}

CDbObject::~CDbObject(void)
{
	if(m_hWaiteHandle) CloseHandle(m_hWaiteHandle);
}


BOOL CDbObject::GetRecordset(LPCTSTR lpszSql, _Recordset **ppRs)
{
	CHECKERROR;
	m_nCmdType = 1;
	m_pRs = *ppRs;
	m_lpszSql = lpszSql;

	ResetEvent(m_hWaiteHandle);
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ExcuteThreadDB,this,0,0);
	if(::WaitForSingleObject(m_hWaiteHandle,5000)!=WAIT_OBJECT_0)
	{
		m_dwLastErrorTime=GetTickCount();
		//::TerminateThread(hThread,0);
		CloseHandle (hThread);
	}
	else m_dwLastErrorTime = 0;
	*ppRs = m_pRs;
	m_bUse = FALSE;
	return m_bRetValue;
}

BOOL CDbObject::UpdateRecordset(_Recordset *pRs)
{
	CHECKERROR;
	m_nCmdType = 2;
	m_pRs = pRs;

	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ExcuteThreadDB,this,0,0);
	if(::WaitForSingleObject(m_hWaiteHandle,3000)!=WAIT_OBJECT_0)
	{
		m_dwLastErrorTime=GetTickCount();
		//::TerminateThread(hThread,0);
		CloseHandle (hThread);
	}
	else m_dwLastErrorTime = 0;
	m_bUse = FALSE;
	return m_bRetValue;
}

BOOL CDbObject::ExecuteCommand(LPCTSTR lpszSql,long &lAffectCount)
{
	CHECKERROR;
	m_nCmdType = 3;
	m_lpszSql = lpszSql;
	m_lAffectCount = lAffectCount;

	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ExcuteThreadDB,this,0,0);
	if(::WaitForSingleObject(m_hWaiteHandle,2000)!=WAIT_OBJECT_0)
	{
		m_dwLastErrorTime=GetTickCount();
		//::TerminateThread(hThread,0);
		CloseHandle (hThread);
	}
	else m_dwLastErrorTime=0;
	lAffectCount = m_lAffectCount;
	m_bUse = FALSE;
	return m_bRetValue;
}

long CDbObject::GetDatabaseType(long& nType)
{
	m_nCmdType = 4;
	m_lAffectCount = -1;

	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ExcuteThreadDB,this,0,0);
	if(::WaitForSingleObject(m_hWaiteHandle,5000)!=WAIT_OBJECT_0)
	{
		m_dwLastErrorTime=GetTickCount();
		//::TerminateThread(hThread,0);
		CloseHandle (hThread);
	}
	else m_dwLastErrorTime = 0;
	nType = m_lAffectCount;
	m_bUse = FALSE;
	return m_bRetValue;
}


void CDbObject::ExcuteThreadDB(LPVOID lpVoid)
{
	CDbObject *pThis = (CDbObject*)lpVoid;

	::CoInitialize(NULL);

	HRESULT	hr;
	CComBSTR bstrSql = pThis->m_lpszSql;
	unsigned long ulAffectCount = 0;
	try
	{
		CComPtr<IStdioDB>	pIStdioDB;
		hr = S_FALSE;
		int i;
		for(i = 0;i < 3;i++)
		{
			hr = pIStdioDB.CoCreateInstance(__uuidof(StdioDB), NULL, CLSCTX_ALL);
			if(hr == S_OK) break;
			if(hr == 0X80010005)
			{
				GetFailInfo();
				goto ReturnFlag;
			}
			Sleep(10);
		}
		if (FAILED(hr))
		{
			ASSERT(NULL == _T("创建IStdioDB对象失败！"));
			goto ReturnFlag;
		}
		switch(pThis->m_nCmdType)
		{
		default:
			goto ReturnFlag;
		case 1:
			hr	=	pIStdioDB->GetRecordset(bstrSql,&pThis->m_pRs);
			if( FAILED(hr)) 
			{
				goto ReturnFlag;
			}
			break;
		case 2:
			hr	=	pIStdioDB->UpdateRecordset(pThis->m_pRs);
			if( FAILED(hr))
			{
				goto ReturnFlag;
			}
			break;
		case 3:
			hr	=	pIStdioDB->ExecuteCommand(bstrSql,&ulAffectCount);
			pThis->m_lAffectCount = ulAffectCount;
			if( FAILED(hr)) 
			{
				goto ReturnFlag;
			}
			break;
		case 4:
			hr	=	pIStdioDB->GetDatabaseType(&pThis->m_lAffectCount);
			break;
		}
		pThis->m_bRetValue = TRUE;
		goto ReturnFlag;
	}
	catch (_com_error &e)
	{
	}
	catch(...)
	{
	}
ReturnFlag:
	SetEvent(pThis->m_hWaiteHandle);
	::CoUninitialize();
	Sleep(5000);
	delete pThis;
}

void CDbObject::GetFailInfo()
{
	CString strMessage;
	HMODULE hModule = ::LoadLibrary(_T("ExceptionHandlerU.dll"));
	if(!hModule)
	{
		strMessage.Format(_T("错误号：0X80010005；错误说明：在消息筛选器里时，不可对外调用！\n（没有找到ExceptionHandlerU.dll文件）"));
		AfxMessageBoxAir(strMessage);
		return;
	}

	BOOL (WINAPI *GetStackWalkerInfo)(LPTSTR ,long);

	(FARPROC&)GetStackWalkerInfo = ::GetProcAddress(hModule,"EH_GetStackWalkerInfo");
	if(!GetStackWalkerInfo)
	{
		::FreeLibrary(hModule);
		strMessage.Format(_T("错误号：0X80010005；错误说明：在消息筛选器里时，不可对外调用！\n（ExceptionHandlerU.dll里函数EH_GetStackWalkerInfo无法定位）"));
		AfxMessageBoxAir(strMessage);
		return;
	}
	TCHAR lpBuffer[2048]; 
	memset(lpBuffer,0,2048*sizeof(TCHAR)); 
	GetStackWalkerInfo(lpBuffer,2047); 
	CString strCallPath = lpBuffer;

	::FreeLibrary(hModule);
	strMessage.Format(_T("错误号：0X80010005；错误说明：在消息筛选器里时，不可对外调用！\n（%s）"),strCallPath);
	AfxMessageBoxAir(strMessage);
}