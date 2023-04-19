// TcpDataComm.cpp: implementation of the CTcpDataComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "MSV_TCPDATAComm.h"
#include "TcpDataComm.h"
//#include "MsgReport.h"
#include <mmsystem.h>
#include "CarryClip.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern CCarryClipApp theApp;
CTcpDataComm::CTcpDataComm()
{
	m_bInit		 = FALSE  ;
	m_bListenRun = TRUE   ;
	m_pSRBuffer  = NULL   ;
	m_bTcpOk     = FALSE  ;
	
	WSADATA     wsaData   ;
	WORD version=MAKEWORD(2,0);
	int ret=WSAStartup(version,&wsaData);
	if(ret!=0)
	{
		theApp.RecordStatus(_T("Initilize Error!\n"));
		m_bInit=FALSE;
		MessageBox(NULL, CStringL(_T("Failed in initial socket")), CStringL(L"Warning"), MB_OK|MB_ICONEXCLAMATION);
	}	
	m_pSRBuffer      = new char[MAX_RECSEND_LEN] ;
	m_dwTimeOut      = 4000 ;                      //Tcp TimeOut time
	m_hListenThread  = NULL ;

}

CTcpDataComm::~CTcpDataComm()
{
	Unit();
	WSACleanup();
	if (m_pSRBuffer)
	{
		delete m_pSRBuffer  ;
		m_pSRBuffer  = NULL ;
	}
}


BOOL CTcpDataComm::Init(CString LocalIP)
{
	if (m_bInit)
	{
		return TRUE ;
	}
	//准备开启监听线程
	if (m_hListenThread && m_hListenThread!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hListenThread);
	}
	
	m_hListenThread  = NULL ;
	m_ListenSocket   = NULL ;
	m_DataCommSocket = NULL ;
	m_sLocalIP       = LocalIP    ;
	m_bInit          = TRUE       ;
	return TRUE ;
}
BOOL CTcpDataComm::Unit()
{
	Stop();
	if (!m_bInit)
	{
		return TRUE ;
	}

	m_bInit = FALSE ;
	return TRUE ;
}
BOOL  CTcpDataComm::StartListen(int& iPort)
{	
	//开始listen之前先进行一次Stop
	CString sLog;
	Stop();
	m_bTcpOk    = FALSE ; 

	if (m_ListenSocket != NULL && m_ListenSocket != INVALID_SOCKET)
	{
		closesocket(m_ListenSocket );
		m_ListenSocket = INVALID_SOCKET;
	}



	m_ListenSocket = socket(AF_INET,SOCK_STREAM,NULL);
	if (m_ListenSocket == INVALID_SOCKET ) 
	{
		sLog.Format(_T("CTcpDataComm::StartListen, \
			Create ListenSocket Failed,LastError = %d\n"),WSAGetLastError());
		theApp.RecordStatus(sLog);
		return FALSE ;
	}

	SOCKADDR_IN   addr_in ;
	addr_in.sin_family = AF_INET ;
	USES_CONVERSION;
	addr_in.sin_addr.s_addr  = inet_addr(W2A(m_sLocalIP))  ;
	addr_in.sin_port         = htons(iPort) ;


	char on = 1;   
	int iret = setsockopt(m_ListenSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(char)); 
	if (iret == -1)
	{
		theApp.RecordStatus(_T("设置失败\n"));
		//AfxMessageBox(_T("设置失败"));
	}


	if (bind(m_ListenSocket,(LPSOCKADDR)&addr_in,sizeof(SOCKADDR_IN))  == SOCKET_ERROR)
	{
		//错误处理
		int iError =WSAGetLastError() ;
		sLog.Format(_T("CTcpDataComm::StartListen, \
			Bind ListenSocket Failed,LastError = %d\n"),iError);
		theApp.RecordStatus(sLog);
//		NMTrace(_T("CarrayLog"), logLevelError, sLog);
		if(WSAEADDRINUSE == iError)  // 端口被占
		{
			theApp.RecordStatus(_T("端口已经被占,自动设定监听端口，重新绑定\n"));
			addr_in.sin_port  = 0 ;
			if(bind(m_ListenSocket,(LPSOCKADDR)&addr_in,sizeof(SOCKADDR_IN))  == SOCKET_ERROR)
			{
				iError =WSAGetLastError() ;
				sLog.Format(_T("重新绑定失败,错误号%d\n"),iError);
				theApp.RecordStatus(sLog);
//				NMTrace(_T("CarrayLog"), logLevelError, sLog);
				closesocket(m_ListenSocket);
				m_ListenSocket = NULL ;
				return FALSE ;
			}
		}
		else			//其他错误不处理，直接返回错误
		{
			closesocket(m_ListenSocket);
			m_ListenSocket = NULL ;		
			return FALSE ;
		}
	}

	int ilen = sizeof(SOCKADDR_IN);
	SOCKADDR_IN   addr_out ;
	if(getsockname(m_ListenSocket,(LPSOCKADDR)&addr_out,&ilen) ==SOCKET_ERROR )
	{
		sLog.Format(_T("GetSocketName Failed,LastErr%d\n"),WSAGetLastError());
		theApp.RecordStatus(sLog);
//		NMTrace(_T("CarrayLog"), logLevelError, sLog);
		closesocket(m_ListenSocket);
		m_ListenSocket = NULL ;
		return FALSE ;
	}
	

	//准备开启监听线程
	if (m_hListenThread)
	{
		CloseHandle(m_hListenThread);
		m_hListenThread = NULL ;
	}
	m_hListenThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)_Listen_CB,this, \
		                           NULL,NULL);
	if (!m_hListenThread)
	{
		theApp.RecordStatus(_T("Start Socket Listen thread Failed\n"));
	//	NMTrace(_T("CarrayLog"), logLevelError, _T("Start Socket Listen thread Failed\n"));
		return FALSE ;
	}
	
	iPort  = ntohs(addr_out.sin_port) ;
//	NMTrace(_T("CarrayLog"), logLevelError, _T("Listen Sokcet Bind success And Listen Thread Ok,IP(%s) Listen iPort(%d)"),m_sLocalIP,iPort);
	
	return TRUE ;
}
BOOL  CTcpDataComm::Connect(CString sRemoteIP,int iRemotPort)
{
	Stop();
	CString  strInfo ;
	CString  sLog ;
	SOCKADDR_IN   addr_in ;
	addr_in.sin_family = AF_INET ;
	USES_CONVERSION; 
	addr_in.sin_addr.s_addr  = inet_addr(W2A(m_sLocalIP.GetBuffer(0)))  ;
	m_sLocalIP.ReleaseBuffer();
	addr_in.sin_port         = 0 ;
	m_bTcpOk                 = FALSE ;
	m_DataCommSocket = socket(AF_INET,SOCK_STREAM,NULL);

	if (bind(m_DataCommSocket,(LPSOCKADDR)&addr_in,sizeof(SOCKADDR_IN))  == SOCKET_ERROR)
	{
		sLog.Format(_T("CTcpDataComm::Connect, \
			Bind  DataSocket Failed,LastError = %d\n"),WSAGetLastError());
		theApp.RecordStatus(sLog);
//		NMTrace(_T("CarrayLog"), logLevelError, sLog);
		closesocket(m_DataCommSocket);
		m_DataCommSocket = NULL ;
		return FALSE ;
	}
	addr_in.sin_family      = AF_INET ;
	addr_in.sin_addr.s_addr  = inet_addr(W2A(sRemoteIP.GetBuffer(0)))  ;
	sRemoteIP.ReleaseBuffer();
	addr_in.sin_port        = htons(iRemotPort) ;

	char on = 1;   
	int iret = setsockopt(m_DataCommSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(char)); 
	if (iret == -1)
	{
		theApp.RecordStatus(_T("\n设置失败"));
		//AfxMessageBox(_T("设置失败"));
	}

	
	/*if(setsockopt(m_DataCommSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&m_dwTimeOut,sizeof(m_dwTimeOut))==SOCKET_ERROR)
	{
		theApp.RecordStatus(_T("设置tcp data socket 发送超时失败\n"));
	}
	if(setsockopt(m_DataCommSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&m_dwTimeOut,sizeof(m_dwTimeOut))==SOCKET_ERROR)
	{
		theApp.RecordStatus(_T("设置tcp data socket 接收超时失败\n"));
	}/**/




	int sock_buf_size = 2*1024*1024; 

	iret = setsockopt( m_DataCommSocket, SOL_SOCKET, SO_SNDBUF, (char *)&sock_buf_size, sizeof(sock_buf_size) ); 
//	if (iret == SOCKET_ERROR)
//		NMTrace(_T("CarrayLog"), logLevelError, _T("setsockopt SO_SNDBUF Failed,iErr(%d)"),WSAGetLastError());
	iret = setsockopt( m_DataCommSocket, SOL_SOCKET, SO_RCVBUF, (char *)&sock_buf_size, sizeof(sock_buf_size) ); 
//	if (iret == SOCKET_ERROR)
//		NMTrace(_T("CarrayLog"), logLevelError, _T("setsockopt SO_RCVBUF Failed,iErr(%d)"),WSAGetLastError());



	//zxy-4-18
	linger lTem ;
	lTem.l_onoff  = 1;
	lTem.l_linger = 1000;
	setsockopt(m_DataCommSocket,SOL_SOCKET,SO_LINGER,(char*)&lTem,sizeof(linger));

	int nTemp  = 1;
	setsockopt(m_DataCommSocket,SOL_SOCKET,SO_DONTROUTE,(char*)&nTemp,sizeof(int));


	int iRes = 0 ;
	do {
		iRes = connect(m_DataCommSocket,(LPSOCKADDR)&addr_in,sizeof(SOCKADDR_IN)) ;
		if (SOCKET_ERROR == iRes)
		{
			sLog.Format(_T("Connect IP %s ,Port %d Failed ,lastErr%d\n"),sRemoteIP,iRemotPort,WSAGetLastError());
//			NMTrace(_T("CarrayLog"), logLevelError, sLog);
		}
		else
			break ;
		Sleep(1000);
	}while(iRes == WSAENETUNREACH || iRes == WSAETIMEDOUT);
	if (iRes ==0 )
	{
		m_bTcpOk = TRUE ;
		sLog.Format(_T("Connect IP %s ,Port %d Success\n"),sRemoteIP,iRemotPort);
		theApp.RecordStatus(sLog);
	//	NMTrace(_T("CarrayLog"), logLevelError, sLog);
	}
	else
	{
		closesocket(m_DataCommSocket);
		m_DataCommSocket = NULL ;
		sLog.Format(_T("Connect IP %s ,Port %d Failed,Return False,lastErr%d\n"),sRemoteIP,iRemotPort,WSAGetLastError());
		theApp.RecordStatus(sLog);
	//	NMTrace(_T("CarrayLog"), logLevelError, sLog);
	}


	return (iRes==0 ? TRUE :FALSE )  ;
}
BOOL CTcpDataComm::SendData(char* pData,int& ilen )
{
	CString sLog ;
	if(!_SleepForTcpOk())
	{
	//	NMTrace(_T("CarrayLog"), logLevelError, _T("CTcpDataComm::SendData Failed for Tcp Not ok"));
		return FALSE ;
	}

	//test
	static DWORD dwPackId = 0 ;

	int iSendLen = ilen ;

	*(int*)m_pSRBuffer = iSendLen ;
	memcpy(m_pSRBuffer +sizeof(int),pData,iSendLen);
	sLog.Format(_T("Len %d\n"),iSendLen);
	theApp.RecordStatus(sLog);


	if(*(int*)m_pSRBuffer  > MAX_RECSEND_LEN - sizeof(int) )
	{
		//log
	//	NMTrace(_T("CarrayLog"), logLevelError, _T("CTcpDataComm::SendData,SendSize(%d) ,len(%d) invalid "),*(int*)m_pSRBuffer,iSendLen);
		return FALSE ;
	}


	int iRes = 0 ,iReSendCnt = 0 ;
	do
	{
		if((iRes = send(m_DataCommSocket,m_pSRBuffer,iSendLen+sizeof(int),NULL)) == SOCKET_ERROR )
		{
			int iErr = WSAGetLastError() ;
			if(iErr == 10060)
				sLog.Format(_T("Send Failed,Socket Send TimeOut,Err=%d\n"),iErr);
			else
				sLog.Format(_T("Send Failed-Warring(%d),Err=%d\n"),iReSendCnt,iErr);
			theApp.RecordStatus(sLog);
		//	NMTrace(_T("CarrayLog"), logLevelError, sLog);
			iReSendCnt++ ;
			Sleep(10);
		}
		else if(iRes != iSendLen+sizeof(int))  //9-10添加的判断
		{
			iRes = SOCKET_ERROR ;
		//	NMTrace(_T("CarrayLog"), logLevelError, _T("Send Failed ,Send Len(%d) !=  the indicated len(%d) "),iRes,iSendLen+sizeof(int));
			break ;
		}else 
		{
			break ;
		}
			
	}while(TRUE && iReSendCnt <2);

	if(iRes == SOCKET_ERROR || iReSendCnt>=2)
	{
		closesocket(m_DataCommSocket);
		m_DataCommSocket = NULL ;
		m_bTcpOk  = FALSE ;
	//	NMTrace(_T("CarrayLog"), logLevelError, _T("Send Failed ,return FALSE"));
		return FALSE ;
	}



	if (iSendLen+4 !=  iRes)
	{
		sLog.Format(_T("Send not ok, Send Size %d, RtnSize=%d"),iSendLen+4, iRes);
	//	NMTrace(_T("CarrayLog"), logLevelError, sLog);
		return FALSE ;
	}

	//test
//	NMTrace(_T("TestTcp"), logLevelWarring,_T("Test :SendData idx(%d),Len(%d)"),dwPackId++,*(int*)m_pSRBuffer);
	return TRUE ;
}
BOOL CTcpDataComm::RecData(char*pData,int& ilen)
{
	CString sLog;
	if(!_SleepForTcpOk())
	{
	//	NMTrace(_T("CarrayLog"), logLevelError, _T("CTcpDataComm::RecData Failed for Tcp Not ok"));
		return FALSE ;
	}

	
	//test
	static DWORD dwPackId = 0 ;

	int iRecLen = 0,iRes = 0 ,iPos = 0  ;
	iRes = recv(m_DataCommSocket,(char*)&iRecLen,4,NULL) ;
	if (SOCKET_ERROR == iRes )
	{
		sLog.Format(_T("RecData  len Failed ,LastErr%d\n"),WSAGetLastError());
	//	NMTrace(_T("CarrayLog"), logLevelError, sLog);
		return FALSE ;
	} 
	if (iRes == 0 || iRecLen<=0)
	{
		ilen = 0;
		sLog.Format(_T("RecData iRes(%d) or iLen(%d)  inFailed ,LastErr%d\n"),iRes,iRecLen,WSAGetLastError());
	//	NMTrace(_T("CarrayLog"), logLevelError, sLog);
		return FALSE;
	} 

	
#define MAX_RECLEN (4096)

	int iRecvSize = MAX_RECLEN;
	do {
		if (iRecLen < MAX_RECLEN)
		{
			iRecvSize = iRecLen;
		}
		iRes = recv(m_DataCommSocket,pData+iPos,iRecvSize,NULL) ;
		if (SOCKET_ERROR == iRes )
		{
			sLog.Format(_T("RecData Data Failed ,iRes(%d),iRecvSize(%d),iPos(%d),pData(%x),pData+iPos(%x),LastErr%d\n"),iRes,iRecvSize,iPos,pData,pData+iPos,WSAGetLastError());
			theApp.RecordStatus(sLog);
			ilen = iPos  ;
		//	NMTrace(_T("CarrayLog"), logLevelError, sLog);
			return FALSE ;
		}

		iPos    += iRes ;
		iRecLen -= iRes ; 
		sLog.Format(_T("Rec Size %d\n"),iRes);
		theApp.RecordStatus(sLog);
	} while(iRecLen >0);
	ilen  = iPos ;

	//test
//	NMTrace(_T("TestTcp"), logLevelWarring,_T("Test :RecData idx(%d),Len(%d)"),dwPackId++,ilen);

	return TRUE ;
}

void CTcpDataComm::Stop()
{
	//9-18
	m_bListenRun = FALSE ;
	if (m_DataCommSocket != NULL && m_DataCommSocket != INVALID_SOCKET)
	{
		closesocket(m_DataCommSocket);
		m_DataCommSocket = NULL ;
	}


	if (m_ListenSocket != NULL && m_ListenSocket != INVALID_SOCKET)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = NULL ;
	}

	if (m_hListenThread) 
	{	
		if(WaitForSingleObject(m_hListenThread,1000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hListenThread,0);
			
			CloseHandle(m_hListenThread);
			m_hListenThread = NULL ;
			theApp.RecordStatus(_T("Kill Thread"));
		}
	}

	m_bTcpOk    = FALSE ;
}

void CTcpDataComm::_Listen_CB(LPVOID para) 
{
	CTcpDataComm* pThis = (CTcpDataComm*) para ;
	pThis->_Listen();
}
void CTcpDataComm::_Listen()
{

	//NMTrace(_T("CarrayLog"), logLevelWarring, _T("CTcpDataComm::_Listen Create Thread(%x) ok"),
//			GetCurrentThreadId());
	USES_CONVERSION ;

	CString strInfo ;
	CString sLog;
//	MsgInfo_S   m_sMsgInfo ;
	if(listen(m_ListenSocket,5) == SOCKET_ERROR )
	{
		//NMTrace(_T("CarrayLog"), logLevelError, _T("Listen Failed Return False ,Err(%d)")
		//	,WSAGetLastError());
	}
	SOCKADDR_IN  addr_in ;
	int          iLen = sizeof(SOCKADDR_IN)   ;
	do {
		if (m_DataCommSocket != NULL && m_DataCommSocket != INVALID_SOCKET)
		{
			closesocket(m_DataCommSocket );
			m_DataCommSocket = INVALID_SOCKET;
		}
		m_DataCommSocket = accept(m_ListenSocket,(LPSOCKADDR)&addr_in,&iLen);
		if (SOCKET_ERROR == m_DataCommSocket )
		{
			sLog.Format(_T("Socket Accept Failed , RemoteIP %s, Port%d, \
			LastErr%d\n"),A2W(inet_ntoa(addr_in.sin_addr)),addr_in.sin_port,WSAGetLastError());
			theApp.RecordStatus(sLog);
			//NMTrace(_T("CarrayLog"), logLevelError, sLog);
		}
		else
			break;
	} while(TRUE && m_bListenRun);
	if (m_DataCommSocket)
	{
		m_bTcpOk = TRUE ;

		sLog.Format(_T("Accetp 成功 建立数据通道,RemoteIP %s, Port%d\n"),A2W(inet_ntoa(addr_in.sin_addr)),addr_in.sin_port);
		theApp.RecordStatus(sLog);
		//NMTrace(_T("CarrayLog"), logLevelInfo, sLog);
	}


	int iret = 0 ,sock_buf_size = 2*1024*1024; 

	iret = setsockopt( m_DataCommSocket, SOL_SOCKET, SO_SNDBUF, (char *)&sock_buf_size, sizeof(sock_buf_size) ); 
//	if (iret == SOCKET_ERROR)
//		NMTrace(_T("CarrayLog"), logLevelError, _T("setsockopt SO_SNDBUF Failed"));
	iret = setsockopt( m_DataCommSocket, SOL_SOCKET, SO_RCVBUF, (char *)&sock_buf_size, sizeof(sock_buf_size) ); 
//		iret = setsockopt( m_DataCommSocket, SOL_SOCKET, SO_RCVBUF, (char *)&sock_buf_size, sizeof(sock_buf_size) ); 
//	if (iret == SOCKET_ERROR)
//		NMTrace(_T("CarrayLog"), logLevelError, _T("setsockopt SO_RCVBUF Failed"));
	

	//zxy-4-18
	linger lTem ;
	lTem.l_onoff  = 1;
	lTem.l_linger = 1000;
	setsockopt(m_DataCommSocket,SOL_SOCKET,SO_LINGER,(char*)&lTem,sizeof(linger));

	/*if(setsockopt(m_DataCommSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&m_dwTimeOut,sizeof(m_dwTimeOut))==SOCKET_ERROR)
	{
		theApp.RecordStatus(_T("设置tcp data socket 发送超时失败\n"));
	}
	if(setsockopt(m_DataCommSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&m_dwTimeOut,sizeof(m_dwTimeOut))==SOCKET_ERROR)
	{
		theApp.RecordStatus(_T("设置tcp data socket 接收超时失败\n"));
	}/**/

	
	//关闭Listen socket 退出listen线程
	closesocket(m_ListenSocket);
	m_ListenSocket  = NULL ;
	theApp.RecordStatus(_T("监听的线程退出\n"));
}

BOOL CTcpDataComm::_SleepForTcpOk()
{
	int iCnt = 0 ;
	while (!m_bTcpOk && iCnt <20)
	{
		Sleep(100);
		iCnt++ ;
	}
	return  m_bTcpOk  ;
}