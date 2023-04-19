// XLSPack.h : main header file for the XLSPack DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CXLSPackApp
// See XLSPack.cpp for the implementation of this class
//
#include "C_XLSPackBundle.h"

class CXLSPackApp : public CWinApp
{
public:
	CXLSPackApp();

// Overrides
public:
	virtual BOOL InitInstance();
	C_XLSPackBundle m_Bundle;
	DECLARE_MESSAGE_MAP()
};
