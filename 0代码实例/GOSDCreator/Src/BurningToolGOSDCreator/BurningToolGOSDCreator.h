
// BurningToolGOSDCreator.h : main header file for the PROJECT_NAME application
//

#pragma once
#pragma comment(lib,"gdiplus.lib") 
#include <GdiPlus.h> 
using namespace Gdiplus;

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBurningToolGOSDCreatorApp:
// See BurningToolGOSDCreator.cpp for the implementation of this class
//

class CBurningToolGOSDCreatorApp : public CWinAppEx
{
public:
	CBurningToolGOSDCreatorApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	void ParseCommandLine(CCommandLineInfo& rCmdInfo);

	BOOL ProcessShellCommand(CCommandLineInfo& rCmdInfo);

// Implementation
	CString m_strBurPrjPath;
	CString m_strChipNamePrefix;
	DWORD	m_dwChipSeries;
	CString m_strBurToolTitle;

	DECLARE_MESSAGE_MAP()
protected:
	ULONG_PTR  m_gdiplusToken;
};

extern CBurningToolGOSDCreatorApp theApp;