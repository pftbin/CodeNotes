
// BurningToolGOSDCreatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BurningToolGOSDCreator.h"
#include "BurningToolGOSDCreatorDlg.h"

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


// CBurningToolGOSDCreatorDlg dialog




CBurningToolGOSDCreatorDlg::CBurningToolGOSDCreatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBurningToolGOSDCreatorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_xMin = 950;
	m_yMin = 500;

	m_nWidth = 992;
	m_nHeight = 500;
}

void CBurningToolGOSDCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_TabCtrl);
}

BOOL CBurningToolGOSDCreatorDlg::OnInitDialog()
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
	InitTabCtrlItemDlg();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CBurningToolGOSDCreatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CBurningToolGOSDCreatorDlg::OnTcnSelchangeTab)
	ON_MESSAGE(WM_USER_UPDATE_PARAMETER,&CBurningToolGOSDCreatorDlg::OnUpdateParameter)
END_MESSAGE_MAP()


// CBurningToolGOSDCreatorDlg message handlers
void CBurningToolGOSDCreatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBurningToolGOSDCreatorDlg::OnPaint()
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
HCURSOR CBurningToolGOSDCreatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBurningToolGOSDCreatorDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	switch(m_TabCtrl.GetCurSel())
	{
	case 0:
		{
			m_ParameterDlg.ShowWindow(SW_HIDE);
			m_CreatorDlg.ShowWindow(SW_SHOW);
			m_CreatorDlg.SetFocus();
			break;
		}
	case 1:
		{
			m_CreatorDlg.ShowWindow(SW_HIDE);
			m_ParameterDlg.ShowWindow(SW_SHOW);			
			m_ParameterDlg.SetFocus();
			break;
		}
	default:
		break;
	}

	*pResult = 0;
}

LRESULT CBurningToolGOSDCreatorDlg::OnUpdateParameter( WPARAM wParam, LPARAM lParam )
{
	if (m_ParameterDlg)
	{
		m_ParameterDlg.UpdateModeltoUI();
	}

	return 0;
}

void CBurningToolGOSDCreatorDlg::InitTabCtrlItemDlg()
{
	m_TabCtrl.SetItemSize(2);

	CRect rs;
	m_TabCtrl.GetClientRect(&rs);
	rs.top+=20;
	rs.bottom-=0;
	rs.left+=0;
	rs.right-=0;

	m_TabCtrl.InsertItem(0,_T("GOSD Creator"));
	m_TabCtrl.InsertItem(1,_T("GOSD Parameters"));
	m_TabCtrl.SetCurSel(0);

	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	m_TabCtrl.GetClientRect( &tabRect);
	m_TabCtrl.GetItemRect(0, &itemRect);

	nX = itemRect.left;
	nY = itemRect.bottom+12;
	nXc = tabRect.right-itemRect.left+10;
	nYc = tabRect.bottom;

	//Creator Dialog
	m_CreatorDlg.Create(IDD_DIALOG_CREATOR,&m_TabCtrl);
	m_CreatorDlg.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_CreatorDlg.SetParent(this);
	m_CreatorDlg.SetModelMgr(&m_GOSDMgr);
	m_CreatorDlg.BringWindowToTop();
	m_CreatorDlg.ShowWindow(SW_SHOW);
	m_CreatorDlg.SetFocus();

	//Parameter Dialog
	m_ParameterDlg.Create(IDD_DIALOG_PARAMETER,&m_TabCtrl);
	m_ParameterDlg.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_ParameterDlg.SetParent(this);
	m_ParameterDlg.SetModelMgr(&m_GOSDMgr);
	m_ParameterDlg.UpdateModeltoUI();
	m_ParameterDlg.BringWindowToTop();
	m_ParameterDlg.ShowWindow(SW_HIDE);
	m_ParameterDlg.SetFocus();

}

