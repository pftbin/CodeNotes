#pragma once
#include "Driver.h"
#include "XWSock.h"
using namespace XWSock;

class CUDPSocket : public ISockHandler
{
public:
	CUDPSocket(UDPSOCKET id);
	~CUDPSocket(void);

	int Open(IUDPHandler* pHandler, void* act = NULL, u_short lport = 0);
	int Close(void);
	int GetHostAddr(u_long& ip, u_short& port);
	int SetSockOpt(int optname, const char *optval, int optlen);
	int SendTo(const char* buf, int len, u_long dip, u_short dport);
	int SendTo(const iovec* vec, int count, u_long* ssize, u_long dip, u_short dport);

	virtual void HandleInput(SOCKET s, void* act);
	virtual void HandleOutput(SOCKET s, void* act);

public:
	static UDPSOCKET CreateSocket(IUDPHandler* pHandler, void* act, u_short lport = 0);
	static int CloseSocket(UDPSOCKET s);
	static int GetHostAddr(UDPSOCKET s, u_long& ip, u_short& port);
	static int SetSockOpt(UDPSOCKET s, int optname, const char *optval, int optlen);
	static int SendTo(UDPSOCKET s, const char* buf, int len, u_long dip, u_short dport);
	static int SendTo(UDPSOCKET s, const iovec* vec, int count, u_long* ssize, u_long dip, u_short dport);

protected:
	CUDPSocket(const CUDPSocket&);
	CUDPSocket& operator=(const CUDPSocket&);

protected:
	static CUDPSocket* Lookup_i(UDPSOCKET s);

private:
	typedef std::map<UDPSOCKET, CUDPSocket*>	SocketMap;
	static Lock s_controlLock;
	static UDPSOCKET s_idBase;
	static SocketMap s_sockets;

private:
	UDPSOCKET	m_sockID;
	Lock	m_lifeLock;
	//Lock	m_sendLock;
	//DWORD	m_inputThreadID;
	SOCKET	m_sock;
	//HANDLE	m_hEvents[2]; //[0]--close event, [1]--send event
	char*	m_rcvBuf;
	struct sockaddr_in m_localAddr;
	IUDPHandler* m_pHandler;
};
