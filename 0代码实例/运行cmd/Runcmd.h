// Runcmd.h : main header file for the RUNCMD application
//

#if !defined(AFX_RUNCMD_H__7A4D9CDD_08C8_40AB_95C9_AACFF1AF5264__INCLUDED_)
#define AFX_RUNCMD_H__7A4D9CDD_08C8_40AB_95C9_AACFF1AF5264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRuncmdApp:
// See Runcmd.cpp for the implementation of this class
//

class CRuncmdApp : public CWinApp
{
public:
	CRuncmdApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuncmdApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRuncmdApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RUNCMD_H__7A4D9CDD_08C8_40AB_95C9_AACFF1AF5264__INCLUDED_)
