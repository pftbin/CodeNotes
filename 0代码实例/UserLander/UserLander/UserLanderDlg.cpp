
// UserLanderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UserLander.h"
#include "UserLanderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CUserLanderDlg dialog




CUserLanderDlg::CUserLanderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserLanderDlg::IDD, pParent)
	, m_strUserName(_T(""))
	, m_strPassWord(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUserLanderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_NAME,m_strUserName);
	DDX_Text(pDX,IDC_EDIT_PASSWORD,m_strPassWord);
}

BEGIN_MESSAGE_MAP(CUserLanderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_REG, &CUserLanderDlg::OnBnClickedButtonReg)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CUserLanderDlg::OnBnClickedButtonLogin)
END_MESSAGE_MAP()


// CUserLanderDlg message handlers

BOOL CUserLanderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	_TCHAR szAppFilePath[_MAX_PATH + 1] = {0};
	GetModuleFileName(AfxGetInstanceHandle(), szAppFilePath, _MAX_PATH);
	CString szFullPath = szAppFilePath;
	szFullPath = szFullPath.Left(szFullPath.ReverseFind(_T('\\')));
	CString szDataBasePath;
	szDataBasePath.Format(_T("%s\\%s"), szFullPath, _T("mydata.db"));

	
	myDataBase.InitDataBase(szDataBasePath);



	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUserLanderDlg::OnClose()
{
	myDataBase.CloseDataBase();
	CDialog::OnClose();
}
void CUserLanderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUserLanderDlg::OnPaint()
{
// 	CRect rect; 
// 	CPaintDC dc(this); 
// 	GetClientRect(rect); 
// 	dc.FillSolidRect(rect,RGB(50,0,200)); //Set Bk Color

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUserLanderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUserLanderDlg::OnBnClickedButtonReg()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_strUserName.CompareNoCase(_T("")) == 0 || m_strPassWord.CompareNoCase(_T("")) == 0)
	{
		AfxMessageBox(_T("Register Failed."));
		return;
	}
	
	BOOL bRet = myDataBase.InsertItem(m_strUserName,m_strPassWord);
	if (bRet)
	{
		AfxMessageBox(_T("Register Success."));
	}
	else
	{
		AfxMessageBox(_T("Register Failed."));
	}


}

void CUserLanderDlg::OnBnClickedButtonLogin()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_strUserName.CompareNoCase(_T("")) == 0 || m_strPassWord.CompareNoCase(_T("")) == 0)
	{
		AfxMessageBox(_T("Login Failed."));
		return;
	}

	BOOL bRet = myDataBase.FindItem(m_strUserName,m_strPassWord);
	if (bRet)
	{
		AfxMessageBox(_T("Login Success."));
		OpenClient();
		ShowWindow(SW_SHOWMINIMIZED);
//		ShowWindow(SW_HIDE);
	}
	else
	{
		AfxMessageBox(_T("Login Failed."));
	}

}

void CUserLanderDlg::OpenClient()
{
	// TODO: Add your control notification handler code here
	_TCHAR szAppFilePath[_MAX_PATH + 1] = {0};
	GetModuleFileName(AfxGetInstanceHandle(), szAppFilePath, _MAX_PATH);
	CString szFullPath = szAppFilePath;
	szFullPath = szFullPath.Left(szFullPath.ReverseFind(_T('\\')));
	szFullPath = szFullPath.Left(szFullPath.ReverseFind(_T('\\')));
	szFullPath = szFullPath.Left(szFullPath.ReverseFind(_T('\\')));

	CString szClientPath;
	szClientPath.Format(_T("%s\\%s"), szFullPath, _T("ThreadMsg\\Debug\\ThreadMsg.exe"));



	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	TCHAR szCommandline[_MAX_PATH + 1] = {0};
	CString strFile;
	strFile.Format(_T("\"%s\""), szClientPath);
	_tcscpy(szCommandline, strFile);

	// Start the child process. 
	if (!CreateProcess(NULL,	// No module name (use command line)
		szCommandline,			// Command line
		NULL,					// Process handle not inheritable
		NULL,					// Thread handle not inheritable
		FALSE,					// Set handle inheritance to FALSE
		0,						// No creation flags
		NULL,					// Use parent's environment block
		szFullPath,				// Use parent's starting directory 
		&si,					// Pointer to STARTUPINFO structure
		&pi)					// Pointer to PROCESS_INFORMATION structure
		) 
	{
		CString szMsgError;
		szMsgError.Format(_T("CreateProcess failed (%d)\n"), GetLastError());
		AfxMessageBox(szMsgError);

		return;
	}
}