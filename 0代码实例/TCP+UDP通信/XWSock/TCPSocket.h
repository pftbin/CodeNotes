#pragma once
#include "Driver.h"
#include "XWSock.h"
using namespace XWSock;

class CTCPSocket : public ISockHandler
{
public:
	CTCPSocket(TCPSOCKET id);
	~CTCPSocket(void);

	int Create(void);
	int Close(void);
	int GetHostAddr(u_long& ip, u_short& port);
	int GetPeerAddr(u_long& ip, u_short& port);
	int SetSockOpt(int level, int optname, const char *optval, int optlen);
	int Listen(ITCPHandler* pHandler, void* act, int backlog, u_short lport, u_long lip = INADDR_ANY);
	int Connect(ITCPHandler* pHandler, void* act, u_long rip, u_short rport, 
		u_long lip = INADDR_ANY, u_short lport = 0, IMsgBlockStrategy* pStrategy = NULL);
	int Send(const char* buf, int len, u_long timeout = 0, int flags = 0);
	int Send(const iovec* vec, int count, u_long* ssize, u_long timeout = 0, int flags = 0);
	int BlockSend(const char* buf, int len, int flags = 0);
	int BlockSend(const iovec* vec, int count, u_long* ssize, int flags = 0);

	virtual void HandleAccept(SOCKET s, void* act);
	virtual void HandleConnect(SOCKET s, void* act, BOOL bConnected);
	virtual void HandleInput(SOCKET s, void* act);
	virtual void HandleOutput(SOCKET s, void* act);
	virtual void HandleClose(SOCKET s, void* act);

public:
	static TCPSOCKET CreateSocket();
	static int CloseSocket(TCPSOCKET s);
	static TCPSOCKET Accept(TCPSOCKET s, ITCPHandler* pHandler, void* act, IMsgBlockStrategy* pStrategy = NULL);
	static int GetHostAddr(TCPSOCKET s, u_long& ip, u_short& port);
	static int GetPeerAddr(TCPSOCKET s, u_long& ip, u_short& port);
	static int SetSockOpt(TCPSOCKET s, int level, int optname, const char *optval, int optlen);
	static int Listen(TCPSOCKET s, ITCPHandler* pHandler, void* act, int backlog, u_short lport, u_long lip = INADDR_ANY);
	static int Connect(TCPSOCKET s, ITCPHandler* pHandler, void* act, u_long rip, u_short rport, 
		u_long lip = INADDR_ANY, u_short lport = 0, IMsgBlockStrategy* pStrategy = NULL);
	static int Send(TCPSOCKET s, const char* buf, int len, u_long timeout = 0, int flags = 0);
	static int Send(TCPSOCKET s, const iovec* vec, int count, u_long* ssize, u_long timeout = 0, int flags = 0);
	static int BlockSend(TCPSOCKET s, const char* buf, int len, int flags = 0);
	static int BlockSend(TCPSOCKET s, const iovec* vec, int count, u_long* ssize, int flags = 0);

protected:
	CTCPSocket(const CTCPSocket&);
	CTCPSocket& operator=(const CTCPSocket&);
	
protected:
	static CTCPSocket* Lookup_i(TCPSOCKET s);

private:
	typedef std::map<TCPSOCKET, CTCPSocket*>	SocketMap;
	static Lock s_controlLock;
	static TCPSOCKET s_idBase;
	static SocketMap s_sockets;

private:
	enum { SS_IDLE, SS_LISTENED, SS_CONNECTED, SS_ACCEPTED };
	TCPSOCKET m_sockID;
	Lock	m_lifeLock;
	//Lock	m_sendLock;
	//DWORD	m_threadID;
	SOCKET	m_sock;
	//HANDLE	m_hEvents[2]; //[0]--close event, [1]--send event
	char*	m_rcvBuf;
	int		m_rcvedLen;
	int		m_rcvBufLen;
	//iovec*	m_iovec;
	//int		m_iovecCount;
	int		m_sockState;
	BOOL*	m_pDelSignal;
	ITCPHandler* m_pHandler;
	IMsgBlockStrategy* m_pStrategy;
	struct sockaddr_in m_localAddr;
	struct sockaddr_in m_remoteAddr;
};
