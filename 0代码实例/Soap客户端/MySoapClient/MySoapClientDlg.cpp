
// MySoapClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MySoapClient.h"
#include "MySoapClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMySoapClientDlg �Ի���




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


// CMySoapClientDlg ��Ϣ�������

BOOL CMySoapClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMySoapClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
			//(�ڵ�����,�����ռ�URL����������URL,�����ռ���)
			CString strNodeName,strNameSpaceURL,strEncStyleURL,strPrefix;
			//��ֵΪName/URL0/URL1/std,��Ӧ������Ľڵ�Ϊ: 
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
			Serializer->WriteString("�ɶ�");

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
