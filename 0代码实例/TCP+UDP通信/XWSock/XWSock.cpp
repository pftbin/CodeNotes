// XWSock.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "XWSock.h"
#include "Driver.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "Debug.h"

//IMPLEMENT_LOGER(NetLoger)

CError g_error;
CSysEventLoger g_sysLoger;

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//INIT_LOGER(NetLoger, _T("LogPath"), _T("SockDriver"));
		g_sysLoger.Open(100, _T("XWSock"));
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		{
			try
			{
				CSockDriver::Instance().ClearSoket();
			}
			catch(...)
			{
				g_sysLoger.Close();
				return TRUE;
			}
			g_sysLoger.Close();
		}
		break;
	}
	return TRUE;
}

namespace XWSock
{
	void SetErrorCode(int iMajor, int iMinor, DWORD iErrno)
	{
		g_error.SetError(iMajor, iMinor, iErrno);
	}

	DWORD GetErrorCode(void)
	{
		return g_error.GetErrorCode();
	}

	const char* GetErrorMessageA(void)
	{
		return g_error.GetErrorMessageA();
	}

	const wchar_t* GetErrorMessageW(void)
	{
		return g_error.GetErrorMessageW();
	}

	UDPSOCKET UDPCreateSocket(IUDPHandler* pHandler, void* act, u_short lport)
	{
		return CUDPSocket::CreateSocket(pHandler, act, lport);
	}

	int UDPCloseSocket(UDPSOCKET s)
	{
		return CUDPSocket::CloseSocket(s);
	}

	int UDPGetHostAddr(UDPSOCKET s, u_long& ip, u_short& port)
	{
		return CUDPSocket::GetHostAddr(s, ip, port);
	}

	int UDPSetSockOpt(UDPSOCKET s, int optname, const char *optval, int optlen)
	{
		return CUDPSocket::SetSockOpt(s, optname, optval, optlen);
	}

	int UDPSendTo(UDPSOCKET s, const char* buf, int len, u_long dip, u_short dport)
	{
		return CUDPSocket::SendTo(s, buf, len, dip, dport);
	}

	int UDPSendTo2(UDPSOCKET s, const iovec* vec, int count, u_long* ssize, u_long dip, u_short dport)
	{
		return CUDPSocket::SendTo(s, vec, count, ssize, dip, dport);
	}

	TCPSOCKET TCPCreateSocket()
	{
		return CTCPSocket::CreateSocket();
	}

	int TCPCloseSocket(TCPSOCKET s)
	{
		return CTCPSocket::CloseSocket(s);
	}

	TCPSOCKET TCPAccept(TCPSOCKET s, ITCPHandler* pHandler, void* act, IMsgBlockStrategy* pStrategy)
	{
		return CTCPSocket::Accept(s, pHandler, act, pStrategy);
	}

	int TCPGetHostAddr(TCPSOCKET s, u_long& ip, u_short& port)
	{
		return CTCPSocket::GetHostAddr(s, ip, port);
	}

	int TCPGetPeerAddr(TCPSOCKET s, u_long& ip, u_short& port)
	{
		return CTCPSocket::GetPeerAddr(s, ip, port);
	}

	int TCPSetSockOpt(TCPSOCKET s, int level, int optname, const char *optval, int optlen)
	{
		return CTCPSocket::SetSockOpt(s, level, optname, optval, optlen);
	}

	int TCPListen(TCPSOCKET s, ITCPHandler* pHandler, void* act, int backlog, u_short lport, u_long lip)
	{
		return CTCPSocket::Listen(s, pHandler, act, backlog, lport, lip);
	}

	int TCPConnect(TCPSOCKET s, ITCPHandler* pHandler, void* act, u_long rip, u_short rport, 
		u_long lip, u_short lport, IMsgBlockStrategy* pStrategy)
	{
		return CTCPSocket::Connect(s, pHandler, act, rip, rport, lip, lport, pStrategy);
	}

	int TCPSend(TCPSOCKET s, const char* buf, int len, u_long timeout, int flags)
	{
		return CTCPSocket::Send(s, buf, len, timeout, flags);
	}

	int TCPSend2(TCPSOCKET s, const iovec* vec, int count, u_long* ssize, u_long timeout, int flags)
	{
		return CTCPSocket::Send(s, vec, count, ssize, timeout, flags);
	}

	int TCPBlockSend(TCPSOCKET s, const char* buf, int len, int flags)
	{
		return CTCPSocket::BlockSend(s, buf, len, flags);
	}

	int TCPBlockSend2(TCPSOCKET s, const iovec* vec, int count, u_long* ssize, int flags)
	{
		return CTCPSocket::BlockSend(s, vec, count, ssize, flags);
	}
}
