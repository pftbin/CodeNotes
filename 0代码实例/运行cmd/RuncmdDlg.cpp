// RuncmdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Runcmd.h"
#include "RuncmdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRuncmdDlg dialog

CRuncmdDlg::CRuncmdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRuncmdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRuncmdDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRuncmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRuncmdDlg)
	DDX_Control(pDX, IDC_LINK, m_MapHyperLink);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRuncmdDlg, CDialog)
	//{{AFX_MSG_MAP(CRuncmdDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuncmdDlg message handlers

BOOL CRuncmdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_MapHyperLink.SetURL("https://blog.csdn.net/weixin_43557209");
	m_MapHyperLink.SetTipText("��ӭ����ptf_bin��BLOG");

	SetDlgItemText(IDC_EDIT2,_T("ping 127.0.0.1"));
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRuncmdDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRuncmdDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRuncmdDlg::OnOK() 
{
	CString strInputCmd;
	GetDlgItemText(IDC_EDIT2,strInputCmd);  //��ȡ�༭���������������
	if (strInputCmd.IsEmpty())
	{
		AfxMessageBox(_T("Please input..."));
		return;
	}

	// TODO: Add extra validation here
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);	
	sa.lpSecurityDescriptor = NULL;			//ʹ��ϵͳĬ�ϵİ�ȫ������	
	sa.bInheritHandle = TRUE;				//�����Ľ��̼̳о��

	HANDLE hRead,hWrite;
	if (!CreatePipe(&hRead,&hWrite,&sa,0))  //���������ܵ�
	{		
		MessageBox(_T("CreatePipe Failed!"),_T("��ʾ"),MB_OK | MB_ICONWARNING);		
		return;
	}
	
	STARTUPINFO si;	
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);	
	GetStartupInfo(&si);	
	si.hStdError = hWrite;	
	si.hStdOutput = hWrite;	 //�´������̵ı�׼�������д�ܵ�һ��
	si.wShowWindow = SW_HIDE;  //���ش���	
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	char cmdline[256]; 
	CString strcmdline;
	strcmdline.Format(_T("cmd /C %s"),strInputCmd);	//����CreateProcessW
	sprintf(cmdline,"%s",strcmdline);				//����CreateProcessA
	if (!CreateProcess(NULL,strcmdline.GetBuffer(),NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))  //�����ӽ���
	{
		MessageBox(_T("CreateProcess Failed!"),_T("��ʾ"),MB_OK | MB_ICONWARNING);		
		return;
	}
	CloseHandle(hWrite);  //�رչܵ����
	
	char buffer[4096] = {0};
	CString strOutput;
	DWORD bytesRead;
	while (true) 
	{
		if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)  //��ȡ�ܵ�
			break;
		
		strOutput += buffer;
		SetDlgItemText(IDC_EDIT1,strOutput);  //��ʾ�����Ϣ���༭��,��ˢ�´���
		UpdateWindow();
		Sleep(100);
	}
	CloseHandle(hRead);
}
