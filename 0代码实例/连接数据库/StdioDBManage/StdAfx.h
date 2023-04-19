// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__727E1C58_2CA3_4E91_913B_BE9DD7F3318D__INCLUDED_)
#define AFX_STDAFX_H__727E1C58_2CA3_4E91_913B_BE9DD7F3318D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VerDefine.h"
#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#import "C:\Program Files\Common Files\System\ado\msado15.dll"\
    rename_namespace("AdoNS")\
    rename("EOF","adoEOF")
    using namespace AdoNS;

extern STDMETHODIMP WriteLog(BSTR bstrErrMsg, BSTR bstrHostName, ULONG ulUID, BSTR bstrModuleName, BSTR bstrFileName, ULONG ulLine, BSTR bstrAuthorName, BSTR bstrRemark);
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__727E1C58_2CA3_4E91_913B_BE9DD7F3318D__INCLUDED)
