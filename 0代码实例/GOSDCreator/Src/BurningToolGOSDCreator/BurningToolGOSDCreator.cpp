
// BurningToolGOSDCreator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BurningToolGOSDCreator.h"
#include "BurningToolGOSDCreatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBurningToolGOSDCreatorApp

BEGIN_MESSAGE_MAP(CBurningToolGOSDCreatorApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBurningToolGOSDCreatorApp construction

CBurningToolGOSDCreatorApp::CBurningToolGOSDCreatorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBurningToolGOSDCreatorApp object

CBurningToolGOSDCreatorApp theApp;


// CBurningToolGOSDCreatorApp initialization

BOOL CBurningToolGOSDCreatorApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	
	HANDLE hObject = ::CreateMutex(NULL,FALSE, _T("Mutex_GOSD_Creator_tool"));  
	if (GetLastError() == ERROR_ALREADY_EXISTS)  
	{  
		CloseHandle(hObject);  
		AfxMessageBox(_T("Application is still running."), MB_ICONINFORMATION | MB_OK);  
		return FALSE;  
	}
	
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE; 

	CBurningToolGOSDCreatorDlg dlg;
	m_pMainWnd = &dlg;
	m_strBurPrjPath = cmdInfo.m_strFileName;
	m_strChipNamePrefix = cmdInfo.m_strPortName;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CBurningToolGOSDCreatorApp::ParseCommandLine( CCommandLineInfo& rCmdInfo )
{
	m_dwChipSeries = 0x00;
	CWinApp::ParseCommandLine(rCmdInfo);

	if (__argc == 2)
	{
		rCmdInfo.m_strFileName = __targv[1];
	}
	else if ( __argc == 5)							//<.cb_116768, Support Chip Series
	{
		rCmdInfo.m_strFileName = __targv[1];
		CString strChip = __targv[2];
		char strChipId[256];
		WideCharToMultiByte( CP_ACP, 0, strChip.GetBuffer(), -1, strChipId, 256, NULL, NULL );
		m_dwChipSeries = atoi(strChipId);
		rCmdInfo.m_strPortName = __targv[3]; 
		m_strBurToolTitle = __targv[4];
		//.>
	}

	rCmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
}

BOOL CBurningToolGOSDCreatorApp::ProcessShellCommand( CCommandLineInfo& rCmdInfo )
{
	return CWinApp::ProcessShellCommand(rCmdInfo);
}

BOOL CBurningToolGOSDCreatorApp::ExitInstance()
{
	GdiplusShutdown(m_gdiplusToken);

	return CWinApp::ExitInstance();
}