/* -*- C++ -*- */

//=============================================================================
/**
*  @file    DriverThread.h
*
*  DriverThread.h,v 1.00 2006/12/23
*
*  @author 谢文 <xiewen@163.com>
*/
//=============================================================================

#pragma once
#include "ISockDriver.h"
#include <vector>
#include <map>

class CDriverThread
{
public:
	CDriverThread(void);
	~CDriverThread(void);

	int AddSocket(SOCKET s, void* act, ISockHandler* pHandler, long lNetworkEvents = FD_READ|FD_WRITE);
	int RemoveSoket(SOCKET s);
	int ClearSoket(void);
	int GetSocketCount(void) { return m_nSockCount; }
	bool IsSocketFull(void) { return m_nSockCount >= MAXIMUM_WAIT_OBJECTS-2; }
	bool IsSocketEmpty(void) { return m_nSockCount <= 0; }
	DWORD GetThreadID(void) { return m_dwThreadID; }

protected:
	void CheckCreateThread_i(void);
	void CheckCloseThread_i(void);
	unsigned HandleThreadProc(void);
	static unsigned __stdcall ThreadProc(void* pParam);

protected:
	struct SockInfo {
		SOCKET	sock;
		HANDLE	event;
		void*	act;
		ISockHandler* pHandler;
	};

	class CAutoSetAdjust
	{
		HANDLE m_hStart, m_hStop;
	public:
		CAutoSetAdjust(HANDLE hStart, HANDLE hStop)
		{
			m_hStart = hStart; m_hStop = hStop;
			WSAResetEvent(m_hStop); // 设置成未结束调整
			WSASetEvent(hStart); // 设置成开始调整
		}
		~CAutoSetAdjust()
		{
			WSAResetEvent(m_hStart); // 取消开始调整
			WSASetEvent(m_hStop); // 结束调整
		}
	};

protected:
	struct NetResult {
		BOOL			bValid;
		SockInfo*		pSockInfo;
	};
	struct ConnectResult : public NetResult {
		BOOL			bConnected;
	};
	typedef NetResult	InputResult;
	typedef NetResult	OutputResult;
	typedef NetResult	AcceptResult;
	typedef NetResult	ClosedResult;
	typedef std::map<HANDLE, SockInfo>		SockMap;

	std::vector<InputResult> m_inputSocks;
	std::vector<OutputResult> m_outputSocks;
	std::vector<AcceptResult> m_acceptSocks;
	std::vector<ClosedResult> m_closedSocks;
	std::vector<ConnectResult> m_connectSocks;

	SockMap					m_sockMap;
	std::vector<HANDLE>		m_events;//[MAXIMUM_WAIT_OBJECTS];
	HANDLE					m_hKillEvent;
	HANDLE					m_hStartAdjustEvent;
	HANDLE					m_hStopAdjustEvent;
	//HANDLE*					m_pStartEvent;
	int						m_nSockCount;
	BOOL*					m_pExitSignal;
	HANDLE					m_hThread;
	DWORD					m_dwThreadID;
	Lock					m_ctrlLock;
	Lock					m_adjustLock;
};
