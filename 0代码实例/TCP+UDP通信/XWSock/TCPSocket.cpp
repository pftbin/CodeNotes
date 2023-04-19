#include "StdAfx.h"
#include ".\tcpsocket.h"

#define RCVBUF_SIZE		20480
#define MAX_MSGSIZE		10485760

Lock CTCPSocket::s_controlLock;
CTCPSocket::SocketMap CTCPSocket::s_sockets;
TCPSOCKET CTCPSocket::s_idBase = 0;

CTCPSocket::CTCPSocket(TCPSOCKET id)
: m_sock(INVALID_SOCKET)
, m_pHandler(NULL)
, m_pStrategy(NULL)
, m_rcvedLen(0)
, m_sockID(id)
, m_sockState(SS_IDLE)
, m_pDelSignal(NULL)
//, m_threadID(-1)
{
	//m_hEvents[0] = INVALID_HANDLE_VALUE;
	//m_hEvents[1] = INVALID_HANDLE_VALUE;

	memset(&m_localAddr, 0, sizeof(m_localAddr));
	memset(&m_remoteAddr, 0, sizeof(m_remoteAddr));

	m_rcvBuf = new char[RCVBUF_SIZE];
	m_rcvBufLen = RCVBUF_SIZE;

	//m_iovecCount = 10;
	//m_iovec = new iovec[m_iovecCount];
}

CTCPSocket::~CTCPSocket(void)
{
	Close();
	delete [] m_rcvBuf;
	//delete [] m_iovec;
}

int CTCPSocket::GetHostAddr(u_long& ip, u_short& port)
{
	//Guard guard(m_lifeLock);
	ip = m_localAddr.sin_addr.s_addr;
	port = ntohs(m_localAddr.sin_port);
	return 0;
}

int CTCPSocket::GetPeerAddr(u_long& ip, u_short& port)
{
	//Guard guard(m_lifeLock);
	ip = m_remoteAddr.sin_addr.s_addr;
	port = ntohs(m_remoteAddr.sin_port);
	return 0;
}

int CTCPSocket::Create(void)
{
	Guard guard(m_lifeLock);

	if(m_sock != INVALID_SOCKET)
	{
		g_error.SetError(1, 4, 1u);
		return -1;
	}

	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET)
	{
		g_error.SetError(1, 5, WSAGetLastError());
		return -1;
	}

	return 0;
}

int CTCPSocket::Close(void)
{
	Guard guard(m_lifeLock);

	if(m_sock == INVALID_SOCKET)
		return 0;

	if(m_sockState == SS_IDLE)
	{
		::closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return 0;
	}

	if(m_pDelSignal) *m_pDelSignal = TRUE;

	//::SetEvent(m_hEvents[0]);
	CSockDriver::Instance().RemoveSoket(m_sock);
	::closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	m_sockState = SS_IDLE;

	return 0;
}

int CTCPSocket::SetSockOpt(int level, int optname, const char *optval, int optlen)
{
	//Guard guard(m_lifeLock);
	int ret = ::setsockopt(m_sock, level, optname, optval, optlen);
	if(ret == SOCKET_ERROR)
	{
		g_error.SetError(3, 5, 1u);
		return -1;
	}
	return 0;
}

int CTCPSocket::Listen(ITCPHandler* pHandler, void* act, int backlog, u_short lport, u_long lip)
{
	Guard guard(m_lifeLock);

	if(m_sockState != SS_IDLE)
	{
		g_error.SetError(3, 6, 1u);
		return -1;
	}

	m_localAddr.sin_family = AF_INET;
	m_localAddr.sin_port = htons(lport);
	m_localAddr.sin_addr.s_addr = lip;

	int rc = ::bind(m_sock, (struct sockaddr*)&m_localAddr, sizeof m_localAddr);
	if (rc == SOCKET_ERROR)
	{
		g_error.SetError(1, 6, WSAGetLastError());
		return -1;
	}

	rc = ::listen(m_sock, backlog);
	if (rc == SOCKET_ERROR)
	{
		g_error.SetError(3, 7, WSAGetLastError());
		return -1;
	}

	m_pHandler = pHandler;
	m_sockState = SS_LISTENED;

	int addrlen = sizeof(m_localAddr);
	if(getsockname(m_sock, (struct sockaddr*)&m_localAddr, &addrlen) == SOCKET_ERROR)
	{
		g_error.SetError(3, 7, WSAGetLastError());
	}

	char szHost[128];
	if(gethostname(szHost, sizeof(szHost)) != SOCKET_ERROR)
	{
		hostent* host = gethostbyname(szHost);
		m_localAddr.sin_addr.s_addr = *((u_long*)host->h_addr);
	}

	//m_hEvents[0] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	//m_hEvents[1] = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	return CSockDriver::Instance().AddSocket(m_sock, act, this, FD_ACCEPT|FD_CLOSE);
}

//CTCPSocket* CTCPSocket::Accept(ITCPHandler* pHandler, IMsgBlockStrategy* pStrategy)
//{
//	struct sockaddr_in addr;
//	int addrlen = sizeof(addr);
//	SOCKET s = ::accept(m_sock, (struct sockaddr*)&addr, &addrlen);
//	if(s == INVALID_SOCKET)
//	{
//		//WRITE_LOG(NetLoger, _T("Accept TCP socket failed: %d"), WSAGetLastError());
//		return NULL;
//	}
//
//	CTCPSocket* pSock = new CTCPSocket(InterlockedIncrement(&s_idBase));
//	pSock->m_sock = s;
//	pSock->m_hEvents[0] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
//	pSock->m_hEvents[1] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
//	pSock->m_pHandler = pHandler;
//	pSock->m_pStrategy = pStrategy;
//	memcpy(&pSock->m_localAddr, &m_localAddr, sizeof(m_localAddr));
//	memcpy(&pSock->m_remoteAddr, &addr, sizeof(addr));
//
//	if(CSockDriver::Instance().AddSocket(s, pSock, FD_READ|FD_WRITE|FD_CLOSE) != 0)
//	{
//		//WRITE_LOG(NetLoger, _T("Add TCP socket to driver failed."));
//		delete pSock;
//		return NULL;
//	}
//
//	return pSock;
//}

int CTCPSocket::Connect(ITCPHandler* pHandler, void* act, u_long rip, u_short rport, 
						u_long lip, u_short lport, IMsgBlockStrategy* pStrategy)
{
	Guard guard(m_lifeLock);

	if(m_sockState != SS_IDLE && m_sockState != SS_CONNECTED)
	{
		g_error.SetError(3, 8, 1u);
		return -1;
	}

	if(m_sockState == SS_IDLE)
	{
		m_localAddr.sin_family = AF_INET;
		m_localAddr.sin_port = htons(lport);
		m_localAddr.sin_addr.s_addr = lip;

		int rc = ::bind(m_sock, (struct sockaddr*)&m_localAddr, sizeof m_localAddr);
		if (rc == SOCKET_ERROR)
		{
			g_error.SetError(1, 6, WSAGetLastError());
			return -1;
		}

		m_pHandler = pHandler;
		m_pStrategy = pStrategy;
		m_sockState = SS_CONNECTED;

		int addrlen = sizeof(m_localAddr);
		if(getsockname(m_sock, (struct sockaddr*)&m_localAddr, &addrlen) == SOCKET_ERROR)
		{
			g_error.SetError(1, 6, WSAGetLastError());
		}

		char szHost[128];
		if(gethostname(szHost, sizeof(szHost)) != SOCKET_ERROR)
		{
			hostent* host = gethostbyname(szHost);
			m_localAddr.sin_addr.s_addr = *((u_long*)host->h_addr);
		}

		//m_hEvents[0] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		//m_hEvents[1] = ::CreateEvent(NULL, FALSE, FALSE, NULL);

		if(CSockDriver::Instance().AddSocket(m_sock, act, this, FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE) != 0)
		{
			return -1;
		}
	}

	m_remoteAddr.sin_family = AF_INET;
	m_remoteAddr.sin_port = htons(rport);
	m_remoteAddr.sin_addr.s_addr = rip;

	int rc = ::connect(m_sock, (struct sockaddr*)&m_remoteAddr, sizeof m_remoteAddr);
	if (rc == SOCKET_ERROR)
	{
		rc = WSAGetLastError();
		if(rc != WSAEWOULDBLOCK)
		{
			g_error.SetError(3, 9, WSAGetLastError());
			return -1;
		}
	}

	return 0;
}

int CTCPSocket::Send(const char* buf, int len, u_long timeout, int flags)
{
	//Guard guard(m_sendLock);

	int rc = ::send(m_sock, buf, len, flags);
	if (rc == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			if(timeout == 0)
			{
				g_error.SetError(3, 15, WSAGetLastError());
				return -1;
			}
			struct fd_set fdwrite;
			FD_ZERO(&fdwrite);
			FD_SET(m_sock, &fdwrite);
			if(timeout == INFINITE)
			{
				rc = select(0, NULL, &fdwrite, NULL, NULL);
			}
			else
			{
				timeval time;
				time.tv_sec = timeout/1000;
				time.tv_usec = (timeout%1000)*1000;
				rc = select(0, NULL, &fdwrite, NULL, &time);
			}
			if (rc != 1)
			{
				if(rc == 0) g_error.SetError(3, 16, WSAGetLastError());
				else g_error.SetError(3, 3, WSAGetLastError());
				return -1;
			}
			rc = ::send(m_sock, buf, len, flags);
			if (rc == SOCKET_ERROR)
			{
				g_error.SetError(3, 11, WSAGetLastError());
				return -1;
			}
		}
		else
		{
			g_error.SetError(3, 11, WSAGetLastError());
			return -1;
		}
	}
	return rc;
}

int CTCPSocket::Send(const iovec* vec, int count, u_long* ssize, u_long timeout, int flags)
{
	//Guard guard(m_sendLock);

	u_long nBytes;
	int rc = ::WSASend(m_sock, const_cast<LPWSABUF>(vec), count, &nBytes, flags, NULL, NULL);
	if (rc == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			if(timeout == 0)
			{
				g_error.SetError(3, 15, WSAGetLastError());
				return -1;
			}
			struct fd_set fdwrite;
			FD_ZERO(&fdwrite);
			FD_SET(m_sock, &fdwrite);
			if(timeout == INFINITE)
			{
				rc = select(0, NULL, &fdwrite, NULL, NULL);
			}
			else
			{
				timeval time;
				time.tv_sec = timeout/1000;
				time.tv_usec = (timeout%1000)*1000;
				rc = select(0, NULL, &fdwrite, NULL, &time);
			}
			if (rc != 1)
			{
				if(rc == 0) g_error.SetError(3, 16, WSAGetLastError());
				else g_error.SetError(3, 3, WSAGetLastError());
				return -1;
			}
			rc = ::WSASend(m_sock, const_cast<LPWSABUF>(vec), count, &nBytes, flags, NULL, NULL);
			if (rc == SOCKET_ERROR)
			{
				g_error.SetError(3, 11, WSAGetLastError());
				return -1;
			}
		}
		else
		{
			g_error.SetError(3, 11, WSAGetLastError());
			return -1;
		}
	}

	if(ssize) *ssize = nBytes;
	return rc;
}

int CTCPSocket::BlockSend(const char* buf, int len, int flags)
{
	int nSend = 0;
	while(nSend < len)
	{
		int rc = Send(buf+nSend, len-nSend, INFINITE, flags);
		if (rc == SOCKET_ERROR)
		{
			return -1;
		}
		else
		{
			nSend += rc;
		}
	}
	return nSend;
}

int CTCPSocket::BlockSend(const iovec* vec, int count, u_long* ssize, int flags)
{
	u_long nLen = 0;
	u_long nSend = 0;

	iovec vecs[256];
	iovec* pnew = NULL;
	iovec* pvec = vecs;
	if(count > 256)
	{
		pnew = new iovec[count];
		pvec = pnew;
	}
	int i=0;
	for(; i<count; i++)
	{
		pvec[i].buf = vec[i].buf;
		pvec[i].len = vec[i].len;
		nLen += pvec[i].len;
	}

	while(nSend < nLen)
	{
		u_long nBytes;
		int rc = Send(pvec, count, &nBytes, /*INFINITE*/3000, flags);
		if (rc == SOCKET_ERROR)
		{
			delete [] pnew;
			return -1;
		}
		else
		{
			nSend += nBytes;
			u_long nTmp = 0, nLeft = 0;
			for(i=0; i<count; i++)
			{
				nTmp += pvec[i].len;
				if(nTmp > nBytes)
				{
					nLeft = nTmp-nBytes;
					break;
				}
			}
			pvec += i;
			count -= i;
			if(count > 0)
			{
				pvec[0].buf += (pvec[0].len-nLeft);
				pvec[0].len = nLeft;
			}
		}
	}

	delete [] pnew;
	if(ssize) *ssize = nSend;
	return 0;
}

void CTCPSocket::HandleAccept(SOCKET s, void* act)
{
	//m_threadID = GetCurrentThreadId();
	if(m_pHandler) m_pHandler->HandleAccept(m_sockID, act);
}

void CTCPSocket::HandleConnect(SOCKET s, void* act, BOOL bConnected)
{
	//m_threadID = GetCurrentThreadId();
	if(m_pHandler) m_pHandler->HandleConnect(m_sockID, act, bConnected);
}

void CTCPSocket::HandleInput(SOCKET s, void* act)
{
	//char rcvBuf[2048];
	BOOL bDeleted = FALSE;
	int nOldRcved = m_rcvedLen;

	//m_threadID = GetCurrentThreadId();
	if(m_rcvedLen > MAX_MSGSIZE) return;
	if(m_rcvedLen >= m_rcvBufLen)
	{
		char* buf = new char[m_rcvedLen+ RCVBUF_SIZE];
		memcpy(buf, m_rcvBuf, m_rcvedLen);
		delete [] m_rcvBuf;
		m_rcvBuf = buf;
		m_rcvBufLen = m_rcvedLen + RCVBUF_SIZE;
	}
	int rc = ::recv(m_sock, m_rcvBuf+m_rcvedLen, m_rcvBufLen-m_rcvedLen, 0);
	if(rc <= 0) return;
	m_rcvedLen += rc;

	/*while(true)
	{
	int rc = ::recv(m_sock, rcvBuf, sizeof rcvBuf, 0);
	if(rc <= 0) break;

	m_rcvedLen += rc;
	if(m_rcvedLen > m_rcvBufLen)
	{
	char* buf = new char[m_rcvedLen+1024];
	memcpy(buf, m_rcvBuf, m_rcvedLen-rc);
	delete [] m_rcvBuf;
	m_rcvBuf = buf;
	m_rcvBufLen = m_rcvedLen+1024;
	}
	memcpy(m_rcvBuf+m_rcvedLen-rc, rcvBuf, rc);
	}*/

	m_pDelSignal = &bDeleted;

	if(nOldRcved < m_rcvedLen)
	{
		if(m_pStrategy == NULL)
		{
			if(m_pHandler) m_pHandler->HandleInput(m_sockID, act, m_rcvBuf, m_rcvedLen);
			if(bDeleted) return;
			m_rcvedLen = 0;
		}
		else
		{
			int nMsgLen = 0;
			do {
				nMsgLen = m_pStrategy->GetMsgBlock(m_rcvBuf, m_rcvedLen);
				if(nMsgLen > 0)
				{
					int nMin = min(nMsgLen, m_rcvedLen);
					if(m_pHandler) m_pHandler->HandleInput(m_sockID, act, m_rcvBuf, nMin);
					if(bDeleted) return;
					m_rcvedLen -= nMin;
					memmove(m_rcvBuf, m_rcvBuf+nMin, m_rcvedLen);
				}
			}while(nMsgLen > 0 && m_rcvedLen > 0);
		}
	}

	m_pDelSignal = NULL;
}

void CTCPSocket::HandleOutput(SOCKET s, void* act)
{
	//::SetEvent(m_hEvents[1]);
	if(m_pHandler) m_pHandler->HandleOutput(m_sockID, act);
}

void CTCPSocket::HandleClose(SOCKET s, void* act)
{
	//m_threadID = GetCurrentThreadId();
	if(m_pHandler) m_pHandler->HandleClose(m_sockID, act);
}

//////////////////////////////////////////////////////////////////////////

CTCPSocket* CTCPSocket::Lookup_i(TCPSOCKET s)
{
	Guard guard(s_controlLock);
	SocketMap::iterator iter = s_sockets.find(s);
	if(iter == s_sockets.end()) return NULL;
	return iter->second;
}

TCPSOCKET CTCPSocket::CreateSocket()
{
	CTCPSocket* pSock = new CTCPSocket(InterlockedIncrement(&s_idBase));
	if(pSock->Create() == -1)
	{
		delete pSock;
		return -1;
	}
	else
	{
		Guard guard(s_controlLock);
		s_sockets[pSock->m_sockID] = pSock;
	}
	return pSock->m_sockID;
}

int CTCPSocket::CloseSocket(TCPSOCKET s)
{
	Guard guard(s_controlLock);
	SocketMap::iterator iter = s_sockets.find(s);
	if(iter == s_sockets.end())
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	CTCPSocket* pSock = iter->second;
	pSock->Close();
	delete pSock;
	s_sockets.erase(iter);
	return 0;
}

TCPSOCKET CTCPSocket::Accept(TCPSOCKET s, ITCPHandler* pHandler, void* act, IMsgBlockStrategy* pStrategy)
{
	Guard guard(s_controlLock);
	SocketMap::iterator iter = s_sockets.find(s);
	if(iter == s_sockets.end())
	{
		g_error.SetError(3, 12, 1u);
		return INVALID_TCPSOCKET;
	}

	CTCPSocket* pAcceptor = iter->second;

	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	SOCKET sock = ::accept(pAcceptor->m_sock, (struct sockaddr*)&addr, &addrlen);
	if(s == INVALID_SOCKET)
	{
		g_error.SetError(3, 13, WSAGetLastError());
		if(sock != INVALID_SOCKET)
		{
			closesocket(sock);
		}
		return NULL;
	}

	CTCPSocket* pSock = new CTCPSocket(InterlockedIncrement(&s_idBase));
	pSock->m_sock = sock;
	pSock->m_sockState = SS_ACCEPTED;
	//pSock->m_hEvents[0] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	//pSock->m_hEvents[1] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	pSock->m_pHandler = pHandler;
	pSock->m_pStrategy = pStrategy;
	memcpy(&pSock->m_localAddr, &pAcceptor->m_localAddr, sizeof(pAcceptor->m_localAddr));
	memcpy(&pSock->m_remoteAddr, &addr, sizeof(addr));

	if(CSockDriver::Instance().AddSocket(sock, act, pSock, FD_READ|FD_WRITE|FD_CLOSE) != 0)
	{
		delete pSock;
		return INVALID_TCPSOCKET;
	}

	s_sockets[pSock->m_sockID] = pSock;

	return pSock->m_sockID;
}

int CTCPSocket::GetHostAddr(TCPSOCKET s, u_long& ip, u_short& port)
{
	CTCPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	return pSock->GetHostAddr(ip, port);
}

int CTCPSocket::GetPeerAddr(TCPSOCKET s, u_long& ip, u_short& port)
{
	CTCPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	return pSock->GetPeerAddr(ip, port);
}

int CTCPSocket::SetSockOpt(TCPSOCKET s, int level, int optname, const char *optval, int optlen)
{
	CTCPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	return pSock->SetSockOpt(level, optname, optval, optlen);
}

int CTCPSocket::Listen(TCPSOCKET s, ITCPHandler* pHandler, void* act, int backlog, u_short lport, u_long lip)
{
	CTCPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	return pSock->Listen(pHandler, act, backlog, lport, lip);
}

int CTCPSocket::Connect(TCPSOCKET s, ITCPHandler* pHandler, void* act, u_long rip, u_short rport, 
						u_long lip, u_short lport, IMsgBlockStrategy* pStrategy)
{
	CTCPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	return pSock->Connect(pHandler, act, rip, rport, lip, lport, pStrategy);
}

int CTCPSocket::Send(TCPSOCKET s, const char* buf, int len, u_long timeout, int flags)
{
	CTCPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	return pSock->Send(buf, len, timeout, flags);
}

int CTCPSocket::Send(TCPSOCKET s, const iovec* vec, int count, u_long* ssize, u_long timeout, int flags)
{
	CTCPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	return pSock->Send(vec, count, ssize, timeout, flags);
}

int CTCPSocket::BlockSend(TCPSOCKET s, const char* buf, int len, int flags)
{
	CTCPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	return pSock->Send(buf, len, flags);
}

int CTCPSocket::BlockSend(TCPSOCKET s, const iovec* vec, int count, u_long* ssize, int flags)
{
	CTCPSocket* pSock = Lookup_i(s);
	if(pSock == NULL)
	{
		g_error.SetError(3, 12, 1u);
		return -1;
	}
	return pSock->BlockSend(vec, count, ssize, flags);
}