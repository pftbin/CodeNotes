
// Drag_Drop_OleDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Drag_Drop_OleDemo.h"
#include "Drag_Drop_OleDemoDlg.h"

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


// CDrag_Drop_OleDemoDlg dialog




CDrag_Drop_OleDemoDlg::CDrag_Drop_OleDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrag_Drop_OleDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDrag_Drop_OleDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditSource);
	DDX_Control(pDX, IDC_EDIT2, m_EditDrop);
	DDX_Control(pDX, IDC_BUTTON1, m_ButtonDrop);
	DDX_Control(pDX, IDC_LIST1, m_ListBoxDrop);
	DDX_Control(pDX, IDC_LISTCTRL, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CDrag_Drop_OleDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CDrag_Drop_OleDemoDlg message handlers

BOOL CDrag_Drop_OleDemoDlg::OnInitDialog()
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

	DWORD dwStyle = m_listCtrl.GetExtendedStyle();	            //visible¡¢child¡¢report set in RC
	dwStyle |= LVS_EX_FULLROWSELECT;							//Select HightLight
	dwStyle |= LVS_EX_GRIDLINES;								//GridLines	
	//	dwStyle |= LVS_EX_CHECKBOXES;								//checkbox	
	//	dwStyle |= LVS_EX_INFOTIP;									//ToolTips
	m_listCtrl.SetExtendedStyle(dwStyle);

	CRect rcClient;
	GetClientRect(rcClient);
	m_listCtrl.InsertColumn( 0 ,_T("FileName") , LVCFMT_LEFT);
	m_listCtrl.InsertColumn( 1 ,_T("FileType") , LVCFMT_LEFT);
	m_listCtrl.InsertColumn( 2 ,_T("FileSize") , LVCFMT_LEFT);

	//Set ListCtrl Width
	m_listCtrl.SetColumnWidth(0,(rcClient.Width())/6);
	m_listCtrl.SetColumnWidth(1,(rcClient.Width())/6);
	m_listCtrl.SetColumnWidth(2,LVSCW_AUTOSIZE_USEHEADER);



	if (!m_EditSource.Register())
	{
		TRACE("Edit Register Failed!\n");
	}
	if (!m_EditDrop.Register())
	{
		TRACE("Edit Register Failed!\n");
	}
	if (!m_ButtonDrop.Register())
	{
		TRACE("Button Register Failed!\n");
	}
	if (!m_ListBoxDrop.Register())
	{
		TRACE("ListBox Register Failed!\n");
	}

	if (!m_listCtrl.Register())
	{
		TRACE("ListCtrl Register Failed!\n");
	}

// 	m_EditDrop.SetWindowText(_T("Test"));
// 	m_ButtonDrop.SetWindowText(_T("Test"));
// 	m_EditSource.SetWindowText(_T("DragHere"));
// 	m_ListBoxDrop.AddString(_T("Test"));
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDrag_Drop_OleDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDrag_Drop_OleDemoDlg::OnPaint()
{
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
HCURSOR CDrag_Drop_OleDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

