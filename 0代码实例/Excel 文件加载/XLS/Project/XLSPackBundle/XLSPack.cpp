// XLSPack.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "XLSPack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CXLSPackApp

BEGIN_MESSAGE_MAP(CXLSPackApp, CWinApp)
END_MESSAGE_MAP()


// CXLSPackApp construction

CXLSPackApp::CXLSPackApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CXLSPackApp object

CXLSPackApp theApp;


// CXLSPackApp initialization

BOOL CXLSPackApp::InitInstance()
{
	CWinApp::InitInstance();

	if(!AfxOleInit())  // Your addition starts here.
	{
		AfxMessageBox(_T("Cannot initialize COM dll!"));
		return FALSE;
		// End of your addition.
	}

	return TRUE;
}

BOOL FAR WINAPI GetServiceInstance(LPCTSTR pszServiceType, void** ppInstance)
{
	if (!EMPTY_STR(pszServiceType))
	{
		if (_tcscmp(pszServiceType, _T("I_PiBundle")) == 0)
		{
			*ppInstance = static_cast<void*>(&theApp.m_Bundle);
			return TRUE;
		}
	}

	return FALSE;
}