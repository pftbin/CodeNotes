#include "StdAfx.h"
#include ".\udpsocket.h"

#define RCVBUF_SIZE		65536

Lock CUDPSocket::s_controlLock;
CUDPSocket::SocketMap CUDPSocket::s_sockets;
UDPSOCKET CUDPSocket::s_idBase = 0;

CUDPSocket::CUDPSocket(UDPSOCKET id)
: m_sock(INVALID_SOCKET)
, m_pHandler(NULL)
, m_sockID(id)
//, m_inputThreadID(-1)
{
	//m_hEvents[0] = INVALID_HANDLE_VALUE;
	//m_hEvents[1] = INVALID_HANDLE_VALUE;
	memset(&m_localAddr, 0, sizeof(m_localAddr));

	m_rcvBuf = new char[RCVBUF_SIZE];
}

CUDPSocket::~CUDPSocket(void)
{
	Close();
	delete [] m_rcvBuf;
}

int CUDPSocket::Open(IUDPHandler* pHandler, void* act, u_short lport)
{
	Guard guard(m_lifeLock);

	if(m_sock != INVALID_SOCKET)
	{
		g_error.SetError(1, 1, 1u);
		return -1;
	}

	m_pHandler = pHandler;

	m_sock = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sock == INVALID_SOCKET)
	{
		g_error.SetError(1, 2, WSAGetLastError());
		return -1;
	}

	m_localAddr.sin_family = AF_INET;
	m_localAddr.sin_port = htons(lport);
	m_localAddr.sin_addr.s_addr = INADDR_ANY;

	if(lport != 0)
	{
		int rc = ::bind(m_sock, (struct sockaddr*)&m_localAddr, sizeof m_localAddr);
		if (rc == SOCKET_ERROR)
		{
			g_error.SetError(1, 3, WSAGetLastError());
			::closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			return -1;
		}
	}

	int addrlen = sizeof(m_localAddr);
	if(getsockname(m_sock, (struct sockaddr*)&m_localAddr, &addrlen) == SOCKET_ERROR)
	{
		g_error.SetError(1, 3, WSAGetLastError());
	}

	char szHost[128];
	if(gethostname(szHost, sizeof(szHost)) != SOCKET_ERROR)
	{
		hostent* host = gethostbyname(szHost);
		m_localAddr.sin_addr.s_addr = *((u_long*)host->h_addr);
	}

	//m_hEvents[0] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	//m_hEvents[1] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	try
	{
		return CSockDriver::Instance().AddSocket(m_sock, act, this, FD_READ);
	}
	catch(...)
	{
		return -1;
	}
}

int CUDPSocket::GetHostAddr(u_long& ip, u_short& port)
{
	ip = m_localAddr.sin_addr.s_addr;
	port = ntohs(m_localAddr.sin_port);
	return 0;
}

int CUDPSocket::SetSockOpt(int optname, const char* optval, int optlen)
{
	Guard guard(m_lifeLock);
	int ret = ::setsockopt(m_sock, SOL_SOCKET, optname, optval, optlen);
	if(ret == SOCKET_ERROR)
	{
		g_error.SetError(3, 1, WSAGetLastError());
		return -1;
	}
	return 0;
}

int CUDPSocket::Close(void)
{
	Guard guard(m_lifeLock);

	if(m_sock == INVALID_SOCKET)
		return 0;

	//::SetEvent(m_hEvents[0]);
	try
	{
		if(CSockDriver::Instance().RemoveSoket(m_sock) == -1)
			return -1;
	}
	catch(...)
	{
		::closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return -1;
	}

	::closesocket(m_sock);
	m_sock = INVALID_SOCKET;

	return 0;
}

int CUDPSocket::SendTo(const char* buf, int len, u_long dip, u_short dport)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(dport);
	addr.sin_addr.s_addr = dip;
	memset(addr.sin_zero, 0, sizeof addr.sin_zero);

	//Guard guard(m_sendLock);
	int rc = ::sendto(m_sock, buf, len, 0, (struct sockaddr*)&addr, sizeof(addr));
	if (rc == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			struct fd_set fdwrite;
			FD_ZERO(&fdwrite);
			FD_SET(m_sock, &fdwrite);
			rc = select(0, NULL, &fdwrite, NULL, NULL);
			if (rc != 1)
			{
				g_error.SetError(3, 3, WSAGetLastError());
				return -1;
			}
			rc = ::sendto(m_sock, buf, len, 0, (struct sockaddr*)&addr, sizeof(addr));
			if (rc == SOCKET_ERROR)
			{
				g_error.SetError(3, 3, WSAGetLastError());
				return -1;
			}
		}
		else
		{
			g_error.SetError(3, 3, WSAGetLastError());
			return -1;
		}
	}
	return rc;
}

int CUDPSocket::SendTo(const iovec* vec, int count, u_long* ssize, u_long dip, u_short dport)
{
	u_long nSend = 0;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(dport);
	addr.sin_addr.s_addr = dip;
	memset(addr.sin_zero, 0, sizeof addr.sin_zero);

	//Guard guard(m_sendLock);
	int rc = ::WSASendTo(m_sock, const_cast<LPWSABUF>(vec), count, &nSend, 0, (struct sockaddr*)&addr, sizeof(addr), NULL, NULL);
	if (rc == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			struct fd_set fdwrite;
			FD_ZERO(&fdwrite);
			FD_SET(m_sock, &fdwrite);
			rc = select(0, NULL, &fdwrite, NULL, NULL);
			if (rc != 1)
			{
				g_error.SetError(3, 3, WSAGetLastError());
				return -1;
			}
			rc = ::WSASendTo(m_sock, const_cast<LPWSABUF>(vec), count, &nSend, 0, (struct sockaddr*)&addr, sizeof(addr), NULL, NULL);
			if (rc == SOCKET_ERROR)
			{
				g_error.SetError(3, 3, WSAGetLastError());
				return -1;
			}
		}
		else
		{
			g_error.SetError(3, 3, WSAGetLastError());
			return -1;
		}
	}
	
	if(ssize) *ssize = nSend;
	return rc;
}

void CUDPSocket::HandleInput(SOCKET s, void* act)
{
	//m_inputThreadID = GetCurrentThreadId();
	struct sockaddr_in addr;
	int addrlen = sizeof addr;
	int rc = ::recvfrom(m_sock, m_rcvBuf, RCVBUF_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
	if(rc > 0 && m_pHandler)
	{
		m_pHandler->HandleInput(m_sockID, act, m_rcvBuf, rc, addr.sin_addr.s_addr, ntohs(addr.sin_port));
	}
}

void CUDPSocket::HandleOutput(SOCKET s, void* act)
{
	//::SetEvent(m_hEvents[1]);
}

//////////////////////////////////////////////////////////////////////////

CUDPSocket* CUDPSocket::Lookup_i(UDPSOCKET s)
{
	Guard guard(s_controlLock);
	SocketMap::iterator iter = s_sockets.find(s);
	if(iter == s_sockets.end()) return NULL;
	return iter->second;
}

UDPSOCKET CUDPSocket::CreateSocket(IUDPHandler* pHandler, void* act, u_short lport)
{
	CUDPSocket* pSock = new CUDPSocket(InterlockedIncrement(&s_idBase));
	try
	{
		if(pSock->Open(pHandler, act, lport) == -1)
		{
			delete pSock;
			return INVALID_UDPSOCKET;
		}
		else
		{
			Guard guard(s_controlLock);
			s_sockets[pSock->m_sockID] = pSock;
		}
	}
	catch(...)
	{
		delete pSock;
		return INVALID_UDPSOCKET;
	}
	return pSock->m_sockID;
}

int CUDPSocket::CloseSocket(UDPSOCKET s)
{
	Guard guard(s_controlLock);
	SocketMap::iterator iter = s_sockets.find(s);
	if(iter == s_sockets.end())
	{
		g_error.SetError(3, 4, 1u);
		return -1;
	}
	CUDPSocket* pSock = iter->second;
	pSock->Close();
	delete pSock;
	s_sockets.erase(iter);
	return 0;
}

int CUDPSocket::GetHostAddr(UDPSOCKET s, u_long& ip, u_short& port)
{
	CUDPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 4, 1u);
		return -1;
	}
	return pSock->GetHostAddr(ip, port);
}

int CUDPSocket::SetSockOpt(UDPSOCKET s, int optname, const char *optval, int optlen)
{
	CUDPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 4, 1u);
		return -1;
	}
	return pSock->SetSockOpt(optname, optval, optlen);
}

int CUDPSocket::SendTo(UDPSOCKET s, const char* buf, int len, u_long dip, u_short dport)
{
	CUDPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 4, 1u);
		return -1;
	}
	return pSock->SendTo(buf, len, dip, dport);
}

int CUDPSocket::SendTo(UDPSOCKET s, const iovec* vec, int count, u_long* ssize, u_long dip, u_short dport)
{
	CUDPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 4, 1u);
		return -1;
	}
	return pSock->SendTo(vec, count, ssize, dip, dport);
}
