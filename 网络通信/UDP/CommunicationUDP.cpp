#include "StdAfx.h"
#include "CommunicationUDP.h"

CCommunicationUDP::CCommunicationUDP(void)
: m_pRecvThread(NULL)
, m_bRecvThreadRun(FALSE)
{
	memset(m_byRecvBuffer, 0, sizeof(m_byRecvBuffer));
}

CCommunicationUDP::~CCommunicationUDP(void)
{
}

BOOL CCommunicationUDP::Initialize(LPTSTR lpszLocalIP, int nPort)
{
	int nRet =0;
	WSADATA wsaData;
	SOCKADDR_IN ReceiverAddr;
	if(0 == nPort)
	{
		nPort = 9879;
	}
	nRet=WSAStartup(MAKEWORD(2,2),&wsaData);
	if(nRet!=0)
	{
		//TL_Trace(_T("ClipDestroyer"), logLevelError, _T("Com UDP WSAStartup Failed with Error %d------CCommunicationUDP::Initialize"), nRet);
		return FALSE;
	}

	if ((m_ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
		== INVALID_SOCKET)
	{
		//TL_Trace(_T("ClipDestroyer"), logLevelError, _T("Com UDP  socket Failed with Error %d------CCommunicationUDP::Initialize"), WSAGetLastError());
		WSACleanup();
		return FALSE;
	} 

	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(nPort);
	m_strLocalIP.Format(_T("%s"), lpszLocalIP);
	if(m_strLocalIP.IsEmpty())
	{
		//TL_Trace(_T("ClipDestroyer"), logLevelWarring, _T("Com UDP will bind to any ip------CCommunicationUDP::Initialize"));
		ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		char cCommIp[20];
		memset(cCommIp,0,20);
		wcstombs(cCommIp,m_strLocalIP,20);
		ReceiverAddr.sin_addr.s_addr = inet_addr (cCommIp);
		//TL_Trace(_T("ClipDestroyer"), logLevelInfo, _T("Com UDP will bind to ip %s------CCommunicationUDP::Initialize"), m_strLocalIP);
	}

	if (bind(m_ReceivingSocket, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr))
		== SOCKET_ERROR)
	{
		//TL_Trace(_T("ClipDestroyer"), logLevelError, _T("Com UDP bind Failed with Error %d------CCommunicationUDP::Initialize"), WSAGetLastError());
		closesocket(m_ReceivingSocket);
		WSACleanup();
		return FALSE;
	}

	//TL_Trace(_T("ClipDestroyer"), logLevelInfo, _T("Initialize Success------CCommunicationUDP::Initialize"));

	return TRUE;
}

BOOL CCommunicationUDP::Unintialize()
{	
	closesocket(m_ReceivingSocket);
	WSACleanup();

	//TL_Trace(_T("ClipDestroyer"), logLevelInfo, _T("Uninitialize Success------CCommunicationUDP::Initialize"));

	return TRUE;
}

BOOL CCommunicationUDP::SetCallBack(LPVOID lpCaller, MessageProcessingCallBack lpCallBackFunc)
{
	if(!lpCaller || !lpCallBackFunc)
	{
		//TL_Trace(_T("ClipDestroyer"), logLevelError, _T("SetCallBack Failed------CCommunicationUDP::SetCallBack"));
		return FALSE;
	}

	m_lpCaller = lpCaller;
	m_lpCallBackFunc = lpCallBackFunc;

	//TL_Trace(_T("ClipDestroyer"), logLevelInfo, _T("SetCallBack Success------CCommunicationUDP::SetCallBack"));
	return TRUE;
}

BOOL CCommunicationUDP::StartListen()
{
	AFX_THREADPROC pfnThreadProc = RecvThreadProcCallBack;
	m_pRecvThread = AfxBeginThread(RecvThreadProcCallBack, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
	if(NULL == m_pRecvThread)
	{
		//TL_Trace(_T("ClipDestroyer"), logLevelError, _T("Com UDP begin receive thread Failed------CCommunicationUDP::StartListen"));
		return FALSE;
	}

	m_pRecvThread->m_bAutoDelete = FALSE;
	m_bRecvThreadRun = TRUE;
	m_pRecvThread->ResumeThread();

	//TL_Trace(_T("ClipDestroyer"), logLevelInfo, _T("Start UDP Listen Success------CCommunicationUDP::StartListen"));

	return TRUE;
}

BOOL CCommunicationUDP::StopListen()
{
	if(m_pRecvThread)
	{
		m_bRecvThreadRun = FALSE;
		if(WAIT_TIMEOUT == WaitForSingleObject(m_pRecvThread->m_hThread, 1000))
		{
			//TL_Trace(_T("ClipDestroyer"), logLevelWarring, _T("Wait receive thread to terminate itself timeout------CCommunicationUDP::StopListen"));
		}

		delete m_pRecvThread;
		m_pRecvThread = NULL;
	}

	//TL_Trace(_T("ClipDestroyer"), logLevelInfo, _T("Stop UDP Listen Success------CCommunicationUDP::StopListen"));
	return TRUE;
}

BOOL CCommunicationUDP::SendMessage(LPTSTR lpszDestIP, int iDestPort, const LPBYTE lpbMsg, SIZE_T sizeofmsg)
{
	int ret=0;
	SOCKADDR_IN WriteAddr;
	memset(&WriteAddr, 0, sizeof(WriteAddr));
	WriteAddr.sin_family = AF_INET;
	char szSendTo[30];
	memset(szSendTo,0,30);
	wcstombs(szSendTo,lpszDestIP,20);
	if (strcmp(szSendTo,"") == 0) 
		WriteAddr.sin_addr.s_addr = INADDR_BROADCAST;
	else
		WriteAddr.sin_addr.s_addr = inet_addr(szSendTo);

	WriteAddr.sin_port = htons(iDestPort);

	if(SOCKET_ERROR == sendto(m_ReceivingSocket, (char*)lpbMsg, sizeofmsg, MSG_DONTROUTE,
		(const struct sockaddr *)&WriteAddr, sizeof(SOCKADDR_IN)))
	{
		//TL_Trace(_T("ClipDestroyer"), logLevelError, _T("SendMessage Failed, sendto Failed with Error %d------CCommunicationUDP::SendMessage"),
		//	WSAGetLastError());
		return FALSE;
	}

	//TL_Trace(_T("ClipDestroyer"), logLevelInfo, _T("SendMessage Success------CCommunicationUDP::SendMessage"));

	return TRUE;
}

UINT CCommunicationUDP::RecvThreadProcCallBack(LPVOID lpParam)
{
	CCommunicationUDP* pThis = (CCommunicationUDP*)lpParam;
	return pThis->RecvThreadProc();
}

UINT CCommunicationUDP::RecvThreadProc()
{
	CString strFromIP;
	int nRet = 0;
	SOCKADDR_IN SourceAddr;
	int nSourceAddrLen = sizeof(SourceAddr);
	int nSourcePort = 0;

	while (m_bRecvThreadRun) 
	{
		try
		{
			memset(m_byRecvBuffer,0,sizeof(m_byRecvBuffer));
			nRet = recvfrom(m_ReceivingSocket,(char*)m_byRecvBuffer,MAX_UDP_MSG_SIZE, 0,(struct sockaddr *)&SourceAddr, &nSourceAddrLen);
			if (nRet == SOCKET_ERROR) 
			{
				int nError = GetLastError();
				if ( nError == 10054) 
				{
					//远端关闭,继续运行
					continue;
				}
				else
				{
					//TL_Trace(_T("ClipDestroyer"), logLevelError, _T("Com UDP recvfrom Failed with Error %d------CCommunicationUDP::RecvThreadProc"), nError);
					closesocket(m_ReceivingSocket);
					WSACleanup();
					break;
				}
			}

			strFromIP.Format(_T("%u.%u.%u.%u"),SourceAddr.sin_addr.S_un.S_un_b.s_b1,SourceAddr.sin_addr.S_un.S_un_b.s_b2
				,SourceAddr.sin_addr.S_un.S_un_b.s_b3,SourceAddr.sin_addr.S_un.S_un_b.s_b4);
			nSourcePort = ntohs(SourceAddr.sin_port);

			//TL_Trace(_T("ClipDestroyer"), logLevelInfo, _T("Com UDP recvfrom %s:%d------CCommunicationUDP::RecvThreadProc"), strFromIP, nSourcePort);

			m_lpCallBackFunc(m_byRecvBuffer, nRet, (LPTSTR)(LPCTSTR)strFromIP, nSourcePort, m_lpCaller);
		}
		catch (...)
		{
			//TL_Trace(_T("ClipDestroyer"), logLevelError, _T("Com UDP exception with Error %d------CCommunicationUDP::RecvThreadProc"), GetLastError());
			continue;
		}
	}

	return 0;
}