/* -*- C++ -*- */

//=============================================================================
/**
*  @file    XWSock.h
*
*  XWSock.h,v 1.00 2006/12/23
*
*  @author п╩нд <xiewen@163.com>
*/
//=============================================================================

#ifndef _XWSOCK_H_
#define _XWSOCK_H_

#ifdef XWSOCK_EXPORTS
#define XWSOCK_API extern "C" __declspec(dllexport)
#else
#define XWSOCK_API extern "C" __declspec(dllimport)
#endif

namespace XWSock
{
	typedef long UDPSOCKET;
	typedef long TCPSOCKET;
	typedef WSABUF iovec;

	#define INVALID_UDPSOCKET	-1
	#define INVALID_TCPSOCKET	-1

	#ifdef _UNICODE
	#define GetErrorMessage	GetErrorMessageW
	#else
	#define GetErrorMessage GetErrorMessageA
	#endif

	struct IUDPHandler
	{
		virtual void HandleInput(UDPSOCKET s, void* act, char* buf, int len, u_long src, u_short port) = 0;
	};

	struct ITCPHandler
	{
		virtual void HandleAccept(TCPSOCKET s, void* act) = 0;
		virtual void HandleConnect(TCPSOCKET s, void* act, BOOL bConnected) = 0;
		virtual void HandleInput(TCPSOCKET s, void* act, char* buf, int len) = 0;
		virtual void HandleOutput(TCPSOCKET s, void* act) = 0;
		virtual void HandleClose(TCPSOCKET s, void* act) = 0;
	};

	struct IMsgBlockStrategy
	{
		virtual int GetMsgBlock(const char* buf, int len) = 0;
	};

	// for err
	XWSOCK_API void SetErrorCode(int iMajor, int iMinor, DWORD iErrno = -1);
	XWSOCK_API DWORD GetErrorCode(void);
	XWSOCK_API const char* GetErrorMessageA(void);
	XWSOCK_API const wchar_t* GetErrorMessageW(void);

	// for udp
	XWSOCK_API UDPSOCKET UDPCreateSocket(IUDPHandler* pHandler, void* act = NULL, u_short lport = 0);
	XWSOCK_API int UDPCloseSocket(UDPSOCKET s);
	XWSOCK_API int UDPGetHostAddr(UDPSOCKET s, u_long& ip, u_short& port);
	XWSOCK_API int UDPSetSockOpt(UDPSOCKET s, int optname, const char *optval, int optlen);
	XWSOCK_API int UDPSendTo(UDPSOCKET s, const char* buf, int len, u_long dip, u_short dport);
	XWSOCK_API int UDPSendTo2(UDPSOCKET s, const iovec* vec, int count, u_long* ssize, u_long dip, u_short dport);

	// for tcp
	XWSOCK_API TCPSOCKET TCPCreateSocket();
	XWSOCK_API int TCPCloseSocket(TCPSOCKET s);
	XWSOCK_API TCPSOCKET TCPAccept(TCPSOCKET s, ITCPHandler* pHandler, void* act = NULL, IMsgBlockStrategy* pStrategy = NULL);
	XWSOCK_API int TCPGetHostAddr(TCPSOCKET s, u_long& ip, u_short& port);
	XWSOCK_API int TCPGetPeerAddr(TCPSOCKET s, u_long& ip, u_short& port);
	XWSOCK_API int TCPSetSockOpt(TCPSOCKET s, int level, int optname, const char *optval, int optlen);
	XWSOCK_API int TCPListen(TCPSOCKET s, ITCPHandler* pHandler, void* act, int backlog, u_short lport, u_long lip = INADDR_ANY);
	XWSOCK_API int TCPConnect(TCPSOCKET s, ITCPHandler* pHandler, void* act, u_long rip, u_short rport, 
		u_long lip = INADDR_ANY, u_short lport = 0, IMsgBlockStrategy* pStrategy = NULL);
	XWSOCK_API int TCPSend(TCPSOCKET s, const char* buf, int len, u_long timeout = 0, int flags = 0);
	XWSOCK_API int TCPSend2(TCPSOCKET s, const iovec* vec, int count, u_long* ssize, u_long timeout = 0, int flags = 0);
	XWSOCK_API int TCPBlockSend(TCPSOCKET s, const char* buf, int len, int flags = 0);
	XWSOCK_API int TCPBlockSend2(TCPSOCKET s, const iovec* vec, int count, u_long* ssize, int flags = 0);
}

#endif
