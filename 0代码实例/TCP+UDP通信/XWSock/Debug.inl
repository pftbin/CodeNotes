#ifndef _DEBUG_CPP_
#define _DEBUG_CPP_

#include "Debug.h"

inline CSysEventLoger::CSysEventLoger()
: m_dwID(0), m_hEventSrc(NULL)
{
	memset(m_szExeName, 0, sizeof(m_szExeName));
}

inline CSysEventLoger::~CSysEventLoger()
{
	Close();
}

inline BOOL CSysEventLoger::Open(DWORD dwID, LPCTSTR szSrcName, LPCTSTR szMsgFile)
{
	HKEY hKey = NULL;
	TCHAR szPath[256];
	DWORD dwDisposition, dwAllowed;

	if(m_hEventSrc != NULL) return TRUE;

	GetModuleFileName(NULL, m_szExeName, MAX_PATH);

	lstrcpy(szPath, _T("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\"));
	lstrcat(szPath, szSrcName);

	LONG ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szPath, 0, KEY_READ, &hKey);
	if(ret != ERROR_SUCCESS)
	{
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, szPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
			NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)//为事件源建一个键
		{
			return FALSE;
		}

		if(szMsgFile != NULL)
		{
			lstrcpy(szPath, _T("%SystemRoot%\\System\\"));
			lstrcat(szPath, szMsgFile);
			RegSetValueEx(hKey, _T("EventMessageFile"),0,REG_EXPAND_SZ,(LPBYTE)szPath,(lstrlen(szPath)+1)*sizeof(TCHAR));
		}

		dwAllowed=EVENTLOG_ERROR_TYPE|EVENTLOG_WARNING_TYPE|EVENTLOG_INFORMATION_TYPE;
		RegSetValueEx(hKey,_T("TypesSupported"),0,REG_DWORD,(LPBYTE)&dwAllowed,sizeof(DWORD));

		RegCloseKey(hKey);
	}
	else
	{
		if(szMsgFile != NULL)
		{
			lstrcpy(szPath, _T("%SystemRoot%\\System\\"));
			lstrcat(szPath, szMsgFile);
			RegSetValueEx(hKey, _T("EventMessageFile"),0,REG_EXPAND_SZ,(LPBYTE)szPath,(lstrlen(szPath)+1)*sizeof(TCHAR));
		}

		RegCloseKey(hKey);
	}

	m_dwID = dwID;
	m_hEventSrc = RegisterEventSource(NULL,szSrcName);//指定/打开事件源

	return m_hEventSrc != NULL;
}

inline BOOL CSysEventLoger::Close()
{
	BOOL bRet = TRUE;

	if(m_hEventSrc != NULL)
	{
		bRet = DeregisterEventSource(m_hEventSrc);
		m_hEventSrc = NULL;
	}

	return bRet;
}

inline BOOL CSysEventLoger::Report(LPCTSTR szMsg, WORD wType)
{
	if(m_hEventSrc == NULL) return FALSE;
	return ReportEvent(m_hEventSrc, wType, 0, m_dwID, NULL, 1, 0, &szMsg, NULL);
}

inline BOOL CSysEventLoger::Report2(LPCTSTR szMsg, WORD wType)
{
	if(m_hEventSrc == NULL) return FALSE;

	TCHAR* buf[2];
	buf[0] = m_szExeName;
	buf[1] = const_cast<LPTSTR>(szMsg);
	return ReportEvent(m_hEventSrc, wType, 0, m_dwID, NULL, 2, 0, (LPCTSTR*)buf, NULL);
}

#endif