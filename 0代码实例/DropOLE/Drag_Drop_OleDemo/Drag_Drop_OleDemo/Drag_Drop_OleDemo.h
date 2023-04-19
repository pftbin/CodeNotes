
// Drag_Drop_OleDemo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDrag_Drop_OleDemoApp:
// See Drag_Drop_OleDemo.cpp for the implementation of this class
//

class CDrag_Drop_OleDemoApp : public CWinAppEx
{
public:
	CDrag_Drop_OleDemoApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDrag_Drop_OleDemoApp theApp;