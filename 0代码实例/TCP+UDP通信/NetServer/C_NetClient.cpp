#include "StdAfx.h"
#include "C_NetClient.h"

//数据校验头
//////////////////////////////////////////////////////////////////////////
#define PNP_SYMBOL			0x1111	//固定数据
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
C_NetClient::C_NetClient(void)
{
}

C_NetClient::~C_NetClient(void)
{
}

//////////////////////////////////////////////////////////////////////////
void C_NetClient::SetTCPHandler(void* pTCPHandler)
{
	//TCP事件处理
	m_TCPHandler = pTCPHandler;
}

BOOL C_NetClient::SendTCPMessage(CString strServerIP,CString strServerPort,int nMsgType,CString strMessage)
{
	TCPSOCKET m_tcpClientSock = TCPCreateSocket();
	if(m_tcpClientSock == INVALID_TCPSOCKET)
	{
		TCPCloseSocket(m_tcpClientSock);
		m_tcpClientSock = INVALID_TCPSOCKET;
		return FALSE;
	}

	void*	m_act = NULL;
	u_long	m_svrIP = inet_addr(_bstr_t(strServerIP));
	u_short	m_svrPort = _ttoi(strServerPort);
	if(TCPConnect(m_tcpClientSock, this, m_act, m_svrIP, m_svrPort, INADDR_ANY, 0, this) == -1)
	{
		TCPCloseSocket(m_tcpClientSock);
		m_tcpClientSock = INVALID_TCPSOCKET;
		return FALSE;
	}

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

	if(TCPBlockSend2(m_tcpClientSock, vec, 2, &ssize) == -1)
	{
		TCPCloseSocket(m_tcpClientSock);
		m_tcpClientSock = INVALID_TCPSOCKET;
		if (pBuffer)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
		return FALSE;
	}

	TCPCloseSocket(m_tcpClientSock);
	m_tcpClientSock = INVALID_TCPSOCKET;
	if (pBuffer)
	{
		delete[] pBuffer;
		pBuffer = NULL;
	}
	return TRUE;
}

void C_NetClient::SetUDPHandler(void* pUDPHandler)
{
	//UDP事件处理
	m_UDPHandler = pUDPHandler;
}

BOOL C_NetClient::SendUDPMessage(CString strServerIP,CString strServerPort,int nMsgType,CString strMessage)
{
	UDPSOCKET m_udpClientSock = UDPCreateSocket(this);
	if(m_udpClientSock == INVALID_UDPSOCKET)
	{
		UDPCloseSocket(m_udpClientSock);
		m_udpClientSock = INVALID_UDPSOCKET;
		return FALSE;
	}

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

	u_long	svrIP = inet_addr(_bstr_t(strServerIP));
	u_short	svrPort = _ttoi(strServerPort);
	if (UDPSendTo2(m_udpClientSock, vec, 2, &ssize, svrIP, svrPort))
	{
		UDPCloseSocket(m_udpClientSock);
		m_udpClientSock = INVALID_TCPSOCKET;
		if (pBuffer)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
	}

	UDPCloseSocket(m_udpClientSock);
	m_udpClientSock = INVALID_TCPSOCKET;
	if (pBuffer)
	{
		delete[] pBuffer;
		pBuffer = NULL;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
u_short C_NetClient::Checksum(const u_short *buf, int size)
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
BOOL C_NetClient::IsValidPacket_i(const char* buf, int len)
{
	m_strLogInfo.Format(_T("[IsValidPacket_i]: 进入消息有效性检查"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	LPDGTHDR pHdr = (LPDGTHDR)buf;

	if(len < sizeof(DGTHDR))		return FALSE;
	if(pHdr->symbol != PNP_SYMBOL)	return FALSE;
	if(pHdr->off < sizeof(DGTHDR))	return FALSE;
	if(pHdr->off > pHdr->len)		return FALSE;
	if(pHdr->len > (u_int)len)		return FALSE;

	if(Checksum((u_short*)pHdr, pHdr->off) != 0)
	{
		m_strLogInfo.Format(_T("[IsValidPacket_i]: CheckSum 检查失败退出..."));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		return FALSE;
	}

	m_strLogInfo.Format(_T("[IsValidPacket_i]: 消息有效，检查结束"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	return TRUE;
}

CString C_NetClient::GetStrIP(u_long uip)
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
int  C_NetClient::GetMsgBlock(const char* buf, int len)
{
	m_strLogInfo.Format(_T("[GetMsgBlock]: 进入消息长度检查"));
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
		m_strLogInfo.Format(_T("[GetMsgBlock]: CheckSum 检查失败退出..."));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
		pHdr->checksum = oldsum;
		return len;
	}

	m_strLogInfo.Format(_T("[GetMsgBlock]: 长度有效，检查结束"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	pHdr->checksum = oldsum;
	return pHdr->len;
}

//IUDPHandler
void C_NetClient::HandleInput(UDPSOCKET s, void* act, char* buf, int len, u_long src, u_short port)
{
#if 1
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

#endif

	m_strLogInfo.Format(_T("[HandleInput]: 进入函数"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	if (m_UDPHandler)//收到消息，交给TCPHandler处理
	{
		m_strLogInfo.Format(_T("[HandleInput]: UDPHandler进行处理"));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	}
}
//ITCPHandler
void C_NetClient::HandleAccept(TCPSOCKET s, void* act)
{
	//服务端实现
}

void C_NetClient::HandleConnect(TCPSOCKET s, void* act, BOOL bConnected)
{
	//连接服务器
}

void C_NetClient::HandleInput(TCPSOCKET s, void* act, char* buf, int len)
{
#if 1
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

#endif

	m_strLogInfo.Format(_T("[HandleInput]: 进入函数"));
	//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	if (m_TCPHandler)//收到消息，交给TCPHandler处理
	{
		m_strLogInfo.Format(_T("[HandleInput]: TCPHandler进行处理"));
		//WRITE_LOG(LogerDVInterface, 0, FALSE, m_strLogInfo);
	}
}


void C_NetClient::HandleOutput(TCPSOCKET s, void* act)
{
	//发出消息
}

void C_NetClient::HandleClose(TCPSOCKET s, void* act)
{
	//关闭连接
}
