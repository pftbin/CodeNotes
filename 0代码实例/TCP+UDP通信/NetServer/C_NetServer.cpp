#include "StdAfx.h"
#include "C_NetServer.h"

//����У��ͷ
//////////////////////////////////////////////////////////////////////////
#define PNP_SYMBOL			0x1111	//�̶�����
typedef struct tagDGHDR 
{
	u_int		type;		// type of packet
	u_int		len;		// total length of packet
	u_short     symbol;
	u_long      msg;
	u_short     checksum;	// checksum of header
	u_short     off;		// data offset in the packet
}DGTHDR, *LPDGTHDR;

//////////////////////////////////////////////////////////////////////////
C_NetServer::C_NetServer(void)
{
	//����Winsock
	if (WSAStartup(MAKEWORD(2 , 2) , &m_wsData) != 0) 
	{
		TRACE(("===WSAStartup failed==="));
	}
	m_strLogInfo = _T("");
	m_TCPHandler = NULL;
	m_UDPHandler = NULL;

	m_tcpSvrPort = 10888;
	m_tcpSvrSocket = INVALID_TCPSOCKET;
	InitializeCriticalSection(&m_lockTcpClientArray);
	m_tcpClientArray.clear();

	m_udpSvrPort = 10999;
	m_udpSvrSocket = INVALID_UDPSOCKET;
	InitializeCriticalSection(&m_lockUdpClientArray);
	m_udpClientArray.clear();
}

C_NetServer::~C_NetServer(void)
{
	if (m_tcpSvrSocket != INVALID_TCPSOCKET)
	{
		TCPCloseSocket(m_tcpSvrSocket);
	}
	if (m_udpSvrSocket != INVALID_UDPSOCKET)
	{
		UDPCloseSocket(m_udpSvrSocket);
	}

	//����Winsock
	WSACleanup();    
}

//////////////////////////////////////////////////////////////////////////
BOOL C_NetServer::InitTCPServer(CString strPort)
{
	if(m_tcpSvrSocket != INVALID_TCPSOCKET)
		return TRUE;

	m_tcpSvrSocket = TCPCreateSocket();
	if(m_tcpSvrSocket == INVALID_TCPSOCKET)
	{
		TCPCloseSocket(m_tcpSvrSocket);
		m_tcpSvrSocket = INVALID_TCPSOCKET;

		m_strLogInfo.Format(_T("[InitTCPServer]: %s"), _T("TCPCreateSocket ���ش���..."));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		return FALSE;
	}

	BOOL bNodelay = 1;
	TCPSetSockOpt(m_tcpSvrSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&bNodelay, sizeof(bNodelay));

	m_tcpSvrPort = _ttoi(strPort);
	if(TCPListen(m_tcpSvrSocket, this, NULL, 200, m_tcpSvrPort) != 0)
	{
		TCPCloseSocket(m_tcpSvrSocket);
		m_tcpSvrSocket = INVALID_TCPSOCKET;
		
		m_strLogInfo.Format(_T("[InitTCPServer]: %s,����˿�%d,�Ƿ��ظ�����TCP����"), _T("TCPListen ���ش���..."),m_tcpSvrPort);
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		return FALSE;
	}
	
	u_long tcpSvrIP = 0;
	TCPGetHostAddr(m_tcpSvrSocket,tcpSvrIP,m_tcpSvrPort);
	m_strLogInfo.Format(_T("[InitTCPServer]:TCP��������� %s:%d"), GetStrIP(tcpSvrIP), m_tcpSvrPort);
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);

	m_strLogInfo.Format(_T("[InitTCPServer]: %s"), _T("��ʼ��TCP����ɹ�"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	return TRUE;
}

void C_NetServer::SetTCPHandler(void* pTCPHandler)
{
	//TCP�¼�����
	m_TCPHandler = pTCPHandler;
}

BOOL C_NetServer::Send2TCPClientMsg(void* sock, int nMsgType, CString strMessage)
{
	if(m_tcpSvrSocket == INVALID_TCPSOCKET)
	{
		m_strLogInfo.Format(_T("[Send2TCPClientMsg]: %s"), _T("TCP������׽�����Ч��������..."));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		return FALSE;
	}

	ClientNode* pClient = (ClientNode*)sock;
	BOOL bFind = FALSE;
	EnterCriticalSection(&m_lockTcpClientArray);
	for (std::vector<ClientNode*>::iterator it = m_tcpClientArray.begin(); it != m_tcpClientArray.end(); ++it)
	{	
		if ((*it)->srcip == pClient->srcip)//Client IP Same
		{
			bFind = TRUE;
			break;
		}
	}
	LeaveCriticalSection(&m_lockTcpClientArray);

	BOOL bResult = FALSE;
	if(bFind && pClient)
	{
		BOOL bSend = FALSE;
		for (int i = 0; i < 2; i++)
		{
			if (pClient->tcpClientSocket != INVALID_TCPSOCKET)
			{
				int iBufLen = strMessage.GetLength() * sizeof(TCHAR);
				char* pBuffer = new char[iBufLen+1];
				memset(pBuffer, 0, iBufLen*sizeof(char));
				TCHAR* tBuf = (TCHAR*)strMessage.GetBuffer(0);
				memcpy(pBuffer, tBuf, iBufLen);
				//ParseUTF16BE(pBuffer, iBufLen);
				char* data = pBuffer; 
				int   len = iBufLen;

				DGTHDR hdr;
				hdr.type = nMsgType;
				hdr.len = sizeof(hdr) + len;
				hdr.symbol = PNP_SYMBOL;
				hdr.msg = 0;//default
				hdr.off = sizeof(hdr);
				hdr.checksum = 0;
				hdr.checksum = Checksum((u_short*)&hdr, hdr.off);

				iovec vec[2];
				u_long ssize = sizeof(hdr)+len;
				vec[0].buf = (char*)&hdr;
				vec[0].len = sizeof(hdr);
				vec[1].buf = const_cast<char*>(data);
				vec[1].len = len;

				if(TCPBlockSend2(pClient->tcpClientSocket, vec, 2, &ssize) == -1)
				{
					TCPCloseSocket(pClient->tcpClientSocket);
					pClient->tcpClientSocket = INVALID_TCPSOCKET;
				}

				if (pBuffer)
				{
					delete[] pBuffer;
					pBuffer = NULL;
				}
			}

			if (bSend) break;
			Sleep(100);
		}
	}

	m_strLogInfo.Format(_T("[Send2TCPClientMsg]: ����TCP֪ͨ��Ϣ���: %d, ��Ϣ����: %s "),bResult,strMessage);
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	return bResult;
}


BOOL C_NetServer::InitUDPServer(CString strPort)
{
	if(m_udpSvrSocket != INVALID_UDPSOCKET) 
		return TRUE;

	m_udpSvrPort = _ttoi(strPort);
	m_udpSvrSocket = UDPCreateSocket(this,NULL,m_udpSvrPort);
	if(m_udpSvrSocket == INVALID_UDPSOCKET)
	{
		UDPCloseSocket(m_udpSvrSocket);
		m_udpSvrSocket = INVALID_UDPSOCKET;

		m_strLogInfo.Format(_T("[InitUDPServer]: %s"), _T("UDPCreateSocket ���ش���..."));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		return FALSE;
	}

	BOOL bBroadcast = TRUE;
	UDPSetSockOpt(m_udpSvrSocket, SO_BROADCAST, (char*)&bBroadcast, sizeof(bBroadcast));

	u_long udpSvrIP = 0;
	UDPGetHostAddr(m_udpSvrSocket, udpSvrIP, m_udpSvrPort);
	m_strLogInfo.Format(_T("[InitUDPServer]:UDP��������� %s:%d"), GetStrIP(udpSvrIP), m_udpSvrPort);
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);

	m_strLogInfo.Format(_T("[InitUDPServer]: %s"), _T("��ʼ��UDP����ɹ�"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	return TRUE;
}

void C_NetServer::SetUDPHandler(void* pUDPHandler)
{
	//UDP�¼�����
	m_UDPHandler = pUDPHandler;
}

BOOL C_NetServer::Send2UDPClientMsg(void* sock, int nMsgType, CString strMessage)
{
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
u_short C_NetServer::Checksum(const u_short *buf, int size)
{
	u_long sum = 0;

	if(buf != (u_short *)NULL) 
	{
		while(size > 1) 
		{
			sum += *(buf ++);
			if(sum & 0x80000000)
			{
				sum = (sum & 0xffff) + (sum >> 16);
			}
			size -= sizeof(u_short);
		}    

		if(size)
		{
			sum += (u_short) * (const u_char*) buf;
		}
	}

	while(sum >> 16)
	{
		sum = (sum & 0xffff) + (sum >> 16);
	}
	return (u_short) ~sum;
}
BOOL C_NetServer::IsValidPacket_i(const char* buf, int len)
{
	m_strLogInfo.Format(_T("[IsValidPacket_i]: ������Ϣ��Ч�Լ��"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	LPDGTHDR pHdr = (LPDGTHDR)buf;

	if(len < sizeof(DGTHDR))		return FALSE;
	if(pHdr->symbol != PNP_SYMBOL)	return FALSE;
	if(pHdr->off < sizeof(DGTHDR))	return FALSE;
	if(pHdr->off > pHdr->len)		return FALSE;
	if(pHdr->len > (u_int)len)		return FALSE;

	if(Checksum((u_short*)pHdr, pHdr->off) != 0)
	{
		m_strLogInfo.Format(_T("[IsValidPacket_i]: CheckSum ���ʧ���˳�..."));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		return FALSE;
	}

	m_strLogInfo.Format(_T("[IsValidPacket_i]: ��Ϣ��Ч��������"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	return TRUE;
}

CString C_NetServer::GetStrIP(u_long uip)
{
	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = uip;
	LPSTR str = inet_ntoa(addr.sin_addr);
	CString strIP;
	strIP = str;
	return strIP;
}
//////////////////////////////////////////////////////////////////////////
//IMsgBlockStrategy
int  C_NetServer::GetMsgBlock(const char* buf, int len)
{
	m_strLogInfo.Format(_T("[GetMsgBlock]: ������Ϣ���ȼ��"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	if(len < sizeof(DGTHDR)) return len;

	LPDGTHDR pHdr = (LPDGTHDR)buf;
	if(pHdr->symbol != PNP_SYMBOL)	return len;
	if(pHdr->off < sizeof(DGTHDR))	return len;
	if(pHdr->off > pHdr->len)		return len;
	if((u_int)len < pHdr->off)		return 0;
	if((u_int)len < pHdr->len)		return 0;

	u_short oldsum = pHdr->checksum;
	pHdr->checksum = 0;
	if(Checksum((u_short*)pHdr, pHdr->off) != oldsum)
	{
		m_strLogInfo.Format(_T("[GetMsgBlock]: CheckSum ���ʧ���˳�..."));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		pHdr->checksum = oldsum;
		return len;
	}

	m_strLogInfo.Format(_T("[GetMsgBlock]: ������Ч��������"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	pHdr->checksum = oldsum;
	return pHdr->len;
}

//IUDPHandler
void C_NetServer::HandleInput(UDPSOCKET s, void* act, char* buf, int len, u_long src, u_short port)
{

}
//ITCPHandler
void C_NetServer::HandleAccept(TCPSOCKET s, void* act)
{
	m_strLogInfo.Format(_T("[HandleAccept]: ����������Ӻ���"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	ClientNode* ptcpClient = new ClientNode;
	TCPSOCKET cs = TCPAccept(s, this, ptcpClient, this);
	if(cs != INVALID_TCPSOCKET)
	{
		u_long ip = INADDR_ANY;
		u_short port = 0;

		TCPGetPeerAddr(cs, ip, port);
		ptcpClient->tcpClientSocket = cs;
		ptcpClient->srcip = ip;//�ͻ���IP
		CString strClientIP = GetStrIP(ptcpClient->srcip);

		TCPGetHostAddr(cs, ip, port);
		ptcpClient->dstip = ip;
		ptcpClient->port = port;

		EnterCriticalSection(&m_lockTcpClientArray);
		try
		{
			//update
			std::vector<ClientNode*>::iterator it = m_tcpClientArray.begin();
			for (it; it != m_tcpClientArray.end(); ++it)
			{
				if ((*it)->srcip == ptcpClient->srcip)//�ͻ���IP��ͬ
				{
					(*it)->tcpClientSocket = ptcpClient->tcpClientSocket;
					(*it)->srcip =	ptcpClient->srcip;
					(*it)->dstip =	ptcpClient->dstip;
					(*it)->port =	ptcpClient->port;
					LeaveCriticalSection(&m_lockTcpClientArray);

					m_strLogInfo.Format(_T("[HandleAccept]: �������� %s"),strClientIP);
					//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
					return;
				}
			}

			//new
			m_tcpClientArray.push_back(ptcpClient);
			LeaveCriticalSection(&m_lockTcpClientArray);

			m_strLogInfo.Format(_T("[HandleAccept]: ������ %s"),strClientIP);
			//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		}
		catch(...)
		{
			LeaveCriticalSection(&m_lockTcpClientArray);

			m_strLogInfo.Format(_T("[HandleAccept]: �������ӳ���...[Catch]"));
			//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		}
	}
	else
	{
		m_strLogInfo.Format(_T("[HandleAccept]: �������ӳ���...[TCPAccept==INVALID_TCPSOCKET]"));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);

		delete ptcpClient;
		ptcpClient = NULL;
	}
}

void C_NetServer::HandleConnect(TCPSOCKET s, void* act, BOOL bConnected)
{
	//���ӷ��������ص��˺���
}

void C_NetServer::HandleInput(TCPSOCKET s, void* act, char* buf, int len)
{
	m_strLogInfo.Format(_T("[HandleInput]: ���뺯��"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	if(IsValidPacket_i(buf, len))
	{
		LPDGTHDR pHdr = (LPDGTHDR)buf;
		len = len-pHdr->off;
		buf = buf+pHdr->off;
	}
	//ParseUTF16BE(buf, len);//temp delete
	const char* data = 0;
	int size = len;
	if(size > 0) data = buf;

	CString str;
	int iCount = 0;
#ifdef _UNICODE
	iCount = size/2 + 1;
#else
	iCount = size + 2;
#endif		
	TCHAR* pBuf = new TCHAR[iCount];
	memset(pBuf, 0, iCount * sizeof(TCHAR));
	memcpy(pBuf, data, size);
	str = pBuf;
	delete[] pBuf;

	//�յ���Ϣ������TCPHandler����
	if (m_TCPHandler)
	{
		m_strLogInfo.Format(_T("[HandleInput]: ����HandleTCPMsg���д���"));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	}
}


void C_NetServer::HandleOutput(TCPSOCKET s, void* act)
{
	//������Ϣ���ص��˺���
}

void C_NetServer::HandleClose(TCPSOCKET s, void* act)
{
	//�ر�����ʱ�ѿͻ����׽��ִ�����ɾ��
	m_strLogInfo.Format(_T("[HandleClose]: �����׽��ֹرղ���"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	CString strIP;
	u_long uip;u_short port;
	if(TCPGetPeerAddr(s, uip, port) != -1)
	{
		SOCKADDR_IN addr;
		addr.sin_addr.s_addr = uip;
		strIP = inet_ntoa(addr.sin_addr);
	}

	ClientNode* pClient = (ClientNode*)act;
	if(TCPCloseSocket(s) != -1)
	{
		EnterCriticalSection(&m_lockTcpClientArray);
		try
		{
			vector<ClientNode*>::iterator it = m_tcpClientArray.begin();
			for (it; it != m_tcpClientArray.end(); ++it)
			{
				if ((*it)->srcip == pClient->srcip)//Client IP Same
				{
					m_tcpClientArray.erase(it);
					break;
				}
			}
			LeaveCriticalSection(&m_lockTcpClientArray);
		}
		catch(...)
		{
			LeaveCriticalSection(&m_lockTcpClientArray);
			m_strLogInfo.Format(_T("[HandleClose]: �׽��ֹرղ�������..."));
			//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		}
	}
}
