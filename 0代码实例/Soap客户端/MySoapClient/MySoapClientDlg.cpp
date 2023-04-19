
// MySoapClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MySoapClient.h"
#include "MySoapClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMySoapClientDlg 对话框




CMySoapClientDlg::CMySoapClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMySoapClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMySoapClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMySoapClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CMySoapClientDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CMySoapClientDlg 消息处理程序

BOOL CMySoapClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMySoapClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMySoapClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMySoapClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define SOAP_OK         0
#define SOAP_FAULT		1
#define SOAP_EXCEPTION	2
#define SOAP_UNKNOW     3

void CMySoapClientDlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	int	nRet = SOAP_UNKNOW;
	CString strDetail = _T("");
	try
	{
		CoInitialize(NULL);
		ISoapSerializerPtr	Serializer = NULL;
		ISoapReaderPtr		Reader	   = NULL;
		ISoapConnectorPtr	Connector  = NULL;

		Reader.CreateInstance(__uuidof(SoapReader30));
		HRESULT hr = Connector.CreateInstance(__uuidof(HttpConnector30));
		Serializer.CreateInstance(__uuidof(SoapSerializer30));

		CString strEndPointURL = _T("http://www.webxml.com.cn/WebServices/WeatherWebService.asmx");
		Connector->PutProperty("EndPointURL", (LPCTSTR)strEndPointURL);
		hr = Connector->Connect();

		CString strSoapAction = _T("http://WebXml.com.cn/getWeatherbyCityName");
		Connector->PutProperty("SoapAction" , _bstr_t(strSoapAction));
		hr = Connector->BeginMessage();
		if(SUCCEEDED(hr))
		{
			//(节点名称,命名空间URL，编码类型URL,命名空间名)
			CString strNodeName,strNameSpaceURL,strEncStyleURL,strPrefix;
			//赋值为Name/URL0/URL1/std,对应构造出的节点为: 
			/*
				<std:Name xmlns:std = "URL0" std:encodingStyle="URL1">
					...
				</std:a>
			*/ 

			Serializer->Init(_variant_t((IUnknown*)Connector->InputStream));
			Serializer->StartEnvelope("soap","","");
			Serializer->StartBody("body");

			strNodeName = _T("getWeatherbyCityName");
			strNameSpaceURL = _T("http://WebXml.com.cn/");
			strEncStyleURL = _T("");
			strPrefix = _T("");
			Serializer->StartElement((_bstr_t)strNodeName,(_bstr_t)strNameSpaceURL,(_bstr_t)strEncStyleURL,(_bstr_t)strPrefix);

			strNodeName = _T("theCityName");
			strNameSpaceURL=_T("http://WebXml.com.cn/");
			strEncStyleURL = _T("");
			strPrefix = _T("");
			Serializer->StartElement((_bstr_t)strNodeName,(_bstr_t)strNameSpaceURL,(_bstr_t)strEncStyleURL,(_bstr_t)strPrefix);
			Serializer->WriteString("成都");

			Serializer->EndElement();
			Serializer->EndElement();

			Serializer->EndBody();
			Serializer->EndEnvelope();

			hr = Connector->EndMessage(); 
			if(SUCCEEDED(hr))
			{
				hr = Reader->Load(_variant_t((IUnknown*)Connector->OutputStream), "");
			}
			if(NULL != Reader->Fault)
			{
				CComBSTR bstrReturn;
				Reader->GetFaultString()->get_xml(&bstrReturn);
				strDetail = bstrReturn.m_str;
				nRet = SOAP_FAULT;
			}
			else
			{
				CComBSTR bstrReturn;
				Reader->GetRpcResult()->get_xml(&bstrReturn);
				CComPtr<IXMLDOMElement> pBody = Reader->GetBody();
				BSTR bstrXml;
				pBody->get_xml(&bstrXml);
				strDetail = CString(LPCTSTR(bstrXml));
				nRet = SOAP_OK;
			}
		}
	}
	catch (_com_error &e)
	{
		BSTR bstr = e.Description();
		strDetail.Format(_T("%s"), e.ErrorMessage(), (LPCSTR)(bstr));
		nRet = SOAP_EXCEPTION;
	}
	::CoUninitialize();

	AfxMessageBox(strDetail);
}
