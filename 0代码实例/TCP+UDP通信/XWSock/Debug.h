#ifndef _DEBUG_H_
#define _DEBUG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

inline void DebugDump(LPCTSTR lpszFormat, ...)
{
	TCHAR szBuf[256];
	TCHAR *pBuf = szBuf;

	va_list argList;
	va_start(argList, lpszFormat);
	int nLength = _vsctprintf(lpszFormat, argList);
	if(nLength >= 256) pBuf = new TCHAR[nLength+1];
	_vsntprintf(pBuf, nLength+1, lpszFormat, argList);
	va_end(argList);

	//OutputDebugString(pBuf);

	if(nLength >= 256) delete [] pBuf;
}

#define DEBUG_DUMP	DebugDump

#ifndef TRACE2
#define TRACE2		DebugDump
#endif

class CSysEventLoger
{
public:
	CSysEventLoger();
	~CSysEventLoger();
	BOOL Open(DWORD dwID, LPCTSTR szSrcName, LPCTSTR szMsgFile = NULL);
	BOOL Close();
	BOOL Report(LPCTSTR szMsg, WORD wType);
	BOOL Report2(LPCTSTR szMsg, WORD wType);
private:
	DWORD	m_dwID;
	HANDLE	m_hEventSrc;
	TCHAR	m_szExeName[MAX_PATH];
};

#include "Debug.inl"

#endif