#include "StdAfx.h"
#include ".\driverthread.h"
#include <process.h>
#include "Debug.h"

extern CSysEventLoger g_sysLoger;

CDriverThread::CDriverThread(void)
: m_nSockCount(0)
, m_dwThreadID(-1)
, m_pExitSignal(NULL)
, m_hThread(INVALID_HANDLE_VALUE)
{
	//m_pStartEvent = &m_events[0];
	m_hKillEvent = WSACreateEvent(); // 结束线程事件
	m_hStartAdjustEvent = WSACreateEvent(); // 开始调整sock
	m_hStopAdjustEvent = WSACreateEvent(); // 结束调整sock
	WSASetEvent(m_hStopAdjustEvent);

	m_events.reserve(MAXIMUM_WAIT_OBJECTS);
	m_events.push_back(m_hKillEvent);
	m_events.push_back(m_hStartAdjustEvent);
}

CDriverThread::~CDriverThread(void)
{
	ClearSoket();
	WSACloseEvent(m_hKillEvent);
	WSACloseEvent(m_hStartAdjustEvent);
	WSACloseEvent(m_hStopAdjustEvent);
}

int CDriverThread::AddSocket(SOCKET s, void* act, ISockHandler* pHandler, long lNetworkEvents)
{
	Guard adjustGuard(m_adjustLock);

	if(IsSocketFull())
	{
		g_error.SetError(1, -1, 1u);
		return -1;
	}

	{
		CAutoSetAdjust asa(m_hStartAdjustEvent, m_hStopAdjustEvent);

		{
			Guard ctrlGuard(m_ctrlLock);

			SockMap::iterator iter = m_sockMap.begin();
			for(; iter != m_sockMap.end(); ++iter)
			{
				SockInfo& si = iter->second;
				if(si.sock == s)
				{
					g_error.SetError(1, -1, 1u);
					return -1;
				}
			}

			HANDLE hEvent = WSACreateEvent();
			int rc = WSAEventSelect(s, hEvent, lNetworkEvents);
			if (rc == SOCKET_ERROR)
			{
				g_error.SetError(3, 14, WSAGetLastError());
				WSACloseEvent(hEvent);
				return -1;
			}

			SockInfo si;
			si.sock = s;
			si.event = hEvent;
			si.act = act;
			si.pHandler = pHandler;
			m_sockMap[hEvent] = si;	
			m_events.push_back(hEvent);
			m_nSockCount = (int)m_sockMap.size();
		}
	}

	CheckCreateThread_i();

	return m_nSockCount;
}

int CDriverThread::RemoveSoket(SOCKET s)
{
	Guard adjustGuard(m_adjustLock);

	if(IsSocketEmpty())
	{
		g_error.SetError(2, -1, 1u);
		return -1;
	}

	{
		CAutoSetAdjust asa(m_hStartAdjustEvent, m_hStopAdjustEvent);

		{
			Guard ctrlGuard(m_ctrlLock);
			size_t i=0u;
			for(; i<m_inputSocks.size(); ++i)
			{
				InputResult& ir = m_inputSocks[i];
				if(ir.bValid && ir.pSockInfo->sock == s)
				{
					ir.bValid = FALSE;
					ir.pSockInfo = NULL;
					break;
				}
			}
			for(i=0u; i<m_outputSocks.size(); ++i)
			{
				OutputResult& or = m_outputSocks[i];
				if(or.bValid && or.pSockInfo->sock == s)
				{
					or.bValid = FALSE;
					or.pSockInfo = NULL;
					break;
				}
			}
			for(i=0u; i<m_acceptSocks.size(); ++i)
			{
				AcceptResult& ar = m_acceptSocks[i];
				if(ar.bValid && ar.pSockInfo->sock == s)
				{
					ar.bValid = FALSE;
					ar.pSockInfo = NULL;
					break;
				}
			}
			for(i=0u; i<m_closedSocks.size(); ++i)
			{
				ClosedResult& cr = m_closedSocks[i];
				if(cr.bValid && cr.pSockInfo->sock == s)
				{
					cr.bValid = FALSE;
					cr.pSockInfo = NULL;
					break;
				}
			}
			for(i=0u; i<m_connectSocks.size(); ++i)
			{
				ConnectResult& cr = m_connectSocks[i];
				if(cr.bValid && cr.pSockInfo->sock == s)
				{
					cr.bValid = FALSE;
					cr.pSockInfo = NULL;
					break;
				}
			}

			SockMap::iterator iter = m_sockMap.begin();
			for(; iter != m_sockMap.end(); ++iter)
			{
				SockInfo& si = iter->second;
				if(si.sock == s)
				{
					HANDLE hEvent = iter->first;
					std::vector<HANDLE>::iterator pos = m_events.begin();
					std::advance(pos, 2);
					for(; pos < m_events.end(); ++pos)
					{
						if(*pos == hEvent)
						{
							m_events.erase(pos);
							break;
						}
					}
					WSACloseEvent(hEvent);
					m_sockMap.erase(iter);
					m_nSockCount = (int)m_sockMap.size();
					break;
				}
			}
		}
	}

	//CheckCloseThread_i();

	return m_nSockCount;
}

int CDriverThread::ClearSoket(void)
{
	Guard adjustGuard(m_adjustLock);

	if(IsSocketEmpty()) return 0;

	{
		CAutoSetAdjust asa(m_hStartAdjustEvent, m_hStopAdjustEvent);

		{
			Guard ctrlGuard(m_ctrlLock);

			m_sockMap.clear();
			m_events.clear();
			m_events.push_back(m_hKillEvent);
			m_events.push_back(m_hStartAdjustEvent);
			m_nSockCount = 0;
			
			size_t i=0u;
			for(; i<m_inputSocks.size(); ++i)
			{
				m_inputSocks[i].bValid = FALSE;
				m_inputSocks[i].pSockInfo = NULL;
			}
			for(i=0u; i<m_outputSocks.size(); ++i)
			{
				m_outputSocks[i].bValid = FALSE;
				m_outputSocks[i].pSockInfo = NULL;
			}
			for(i=0u; i<m_acceptSocks.size(); ++i)
			{
				m_acceptSocks[i].bValid = FALSE;
				m_acceptSocks[i].pSockInfo = NULL;
			}
			for(i=0u; i<m_closedSocks.size(); ++i)
			{
				m_closedSocks[i].bValid = FALSE;
				m_closedSocks[i].pSockInfo = NULL;
			}
			for(i=0u; i<m_connectSocks.size(); ++i)
			{
				m_connectSocks[i].bValid = FALSE;
				m_connectSocks[i].pSockInfo = NULL;
			}
		}
	}

	CheckCloseThread_i();

	return m_nSockCount;
}

void CDriverThread::CheckCreateThread_i(void)
{
	if(m_hThread == INVALID_HANDLE_VALUE)
	{
		unsigned int threadId = -1;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, &threadId);
		m_dwThreadID = threadId;
	}
}

void CDriverThread::CheckCloseThread_i(void)
{
	if(m_hThread != INVALID_HANDLE_VALUE)
	{
		if(m_pExitSignal) *m_pExitSignal = TRUE;
		//if(GetCurrentThreadId() != m_dwThreadID)
		//{		
			WSASetEvent(m_events[0]);
			if(WaitForSingleObject(m_hThread, 1000) != WAIT_OBJECT_0)
			{
				::TerminateThread(m_hThread,0u);
			}
		//}
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

unsigned CDriverThread::HandleThreadProc(void)
{
	size_t			i;
	InputResult		inr;
	OutputResult	our;
	AcceptResult	acr;
	ClosedResult	clr;
	ConnectResult	cor;
	BOOL			bExitThread = FALSE;

	m_inputSocks.clear();
	m_outputSocks.clear();
	m_acceptSocks.clear();
	m_closedSocks.clear();
	m_connectSocks.clear();

	m_pExitSignal = &bExitThread;

	while(true)
	{
		WaitForSingleObject(m_hStopAdjustEvent, INFINITE);

		{
			Guard ctrlGuard(m_ctrlLock);

			DWORD dwCount = (DWORD)m_events.size();
			DWORD rc = WaitForMultipleObjects(dwCount, &m_events[0], FALSE, INFINITE);
			if (rc == WAIT_FAILED || rc == WAIT_TIMEOUT)
			{
				////WRITE_LOG(NetLoger, "CDriverThread::HandleThreadProc: WaitForMultipleObjects failed: %d\n", GetLastError());
				return -1;
			}
			else
			{
				rc -= WAIT_OBJECT_0;
				if(rc == 0) return 0;
				else if(rc == 1) continue;

				if(WaitForSingleObject(m_events[0], 0) == WAIT_OBJECT_0)
					return 0;
				else if(WaitForSingleObject(m_events[1], 0) == WAIT_OBJECT_0)
					continue;

				for(size_t i=2u; i<m_events.size(); i++)
				{
					HANDLE hEvent = m_events[i];
					rc = WaitForSingleObject(hEvent, 0);
					if (rc == WAIT_FAILED)
					{
						////WRITE_LOG(NetLoger, "CDriverThread::HandleThreadProc: WaitForSingleObject failed: %d\n", GetLastError());
						return -1;
					}
					else if (rc == WAIT_TIMEOUT)
					{
						// This event isn't signaled, continue to the next one
						continue;
					}

					//if(i == 0) // exit thread
					//{
					//	return 0;
					//}
					//else if(i == 1) // start adjust
					//{
					//	break;
					//}
					//else // its an event associated with a socket that was signaled
					{
						SockMap::iterator iter = m_sockMap.find(hEvent);
						if(iter == m_sockMap.end())
						{
							WSAResetEvent(hEvent);
						}
						else
						{
							SockInfo& si = iter->second;
							WSANETWORKEVENTS nevents = {0};
							WSAWaitForMultipleEvents(1, &hEvent, FALSE, 0, FALSE);

							rc = WSAEnumNetworkEvents(si.sock, hEvent, &nevents);
							if (rc != SOCKET_ERROR)
							{
								if (nevents.lNetworkEvents & FD_READ)
								{
									// Check for read error
									if (nevents.iErrorCode[FD_READ_BIT] == 0)
									{
										inr.bValid = TRUE;
										inr.pSockInfo = &si;
										m_inputSocks.push_back(inr);
										//si.pHandler->HandleInput(si.sock, si.act);
									}
									else
									{
										////WRITE_LOG(NetLoger, "CDriverThread::HandleThreadProc: FD_READ error %d\n", 
										//	nevents.iErrorCode[FD_READ_BIT]);
									}
								}
								if (nevents.lNetworkEvents & FD_WRITE)
								{
									// Check for write error
									if (nevents.iErrorCode[FD_WRITE_BIT] == 0)
									{
										our.bValid = TRUE;
										our.pSockInfo = &si;
										m_outputSocks.push_back(our);
										//si.pHandler->HandleOutput(si.sock, si.act);
									}
									else
									{
										////WRITE_LOG(NetLoger, "CDriverThread::HandleThreadProc: FD_WRITE error %d\n",
										//	nevents.iErrorCode[FD_WRITE_BIT]);
									}
								}
								if (nevents.lNetworkEvents & FD_CLOSE)
								{
									// Check for close error
									if (nevents.iErrorCode[FD_CLOSE_BIT] == 0)
									{
										clr.bValid = TRUE;
										clr.pSockInfo = &si;
										m_closedSocks.push_back(clr);
										//si.pHandler->HandleClose(si.sock, si.act);
									}
									else
									{
										clr.bValid = TRUE;
										clr.pSockInfo = &si;
										m_closedSocks.push_back(clr);
										////WRITE_LOG(NetLoger, "CDriverThread::HandleThreadProc: FD_CLOSE error %d\n",
										//	nevents.iErrorCode[FD_CLOSE_BIT]);
									}
								}
								if (nevents.lNetworkEvents & FD_ACCEPT)
								{
									// Check for close error
									if (nevents.iErrorCode[FD_ACCEPT_BIT] == 0)
									{
										acr.bValid = TRUE;
										acr.pSockInfo = &si;
										m_acceptSocks.push_back(acr);
										//si.pHandler->HandleAccept(si.sock, si.act);
									}
									else
									{
										////WRITE_LOG(NetLoger, "CDriverThread::HandleThreadProc: FD_ACCEPT error %d\n",
										//	nevents.iErrorCode[FD_ACCEPT_BIT]);
									}
								}
								if (nevents.lNetworkEvents & FD_CONNECT)
								{
									// Check for close error
									if (nevents.iErrorCode[FD_CONNECT_BIT] == 0)
									{
										// Socket has been indicated as closing so make sure all the data
										// has been read
										//si.pHandler->HandleConnect(si.sock, si.act, TRUE);
										cor.bValid = TRUE;
										cor.pSockInfo = &si;
										cor.bConnected = TRUE;
										m_connectSocks.push_back(cor);
									}
									else
									{
										cor.bValid = TRUE;
										cor.pSockInfo = &si;
										cor.bConnected = FALSE;
										m_connectSocks.push_back(cor);
										//si.pHandler->HandleConnect(si.sock, si.act, FALSE);
										////WRITE_LOG(NetLoger, "CDriverThread::HandleThreadProc: FD_CONNECT error %d\n",
										//	nevents.iErrorCode[FD_CONNECT_BIT]);
									}
								}
							}
						}
					}
				}
			}
			
			for(i=0u; i<m_inputSocks.size(); ++i)
			{
				UnGuard unGuard(m_ctrlLock);
				try{
					InputResult& ir = m_inputSocks[i];
					SockInfo* si = ir.pSockInfo;
					if(ir.bValid)
					{
						SOCKET sock = si->sock;
						void* act = si->act;					

						si->pHandler->HandleInput(sock, act);

						if(bExitThread)
						{
							ctrlGuard.Abort();
							return 0;
						}
					}
				}
				catch (...) {
					g_sysLoger.Report2(_T("Exception while HandleInput"), EVENTLOG_ERROR_TYPE);
				}
			}
			m_inputSocks.clear();

			for(i=0u; i<m_outputSocks.size(); ++i)
			{
				UnGuard unGuard(m_ctrlLock);
				try{
					OutputResult& or = m_outputSocks[i];
					SockInfo* si = or.pSockInfo;
					if(or.bValid)
					{
						SOCKET sock = si->sock;
						void* act = si->act;


						si->pHandler->HandleOutput(sock, act);

						if(bExitThread)
						{
							ctrlGuard.Abort();
							return 0;
						}

					}
				}
				catch (...) {
					g_sysLoger.Report2(_T("Exception while HandleOutput"), EVENTLOG_ERROR_TYPE);
				}
			}
			m_outputSocks.clear();

			for(i=0u; i<m_acceptSocks.size(); ++i)
			{
				UnGuard unGuard(m_ctrlLock);
				try{
					AcceptResult& ar = m_acceptSocks[i];
					SockInfo* si = ar.pSockInfo;
					if(ar.bValid)
					{
						SOCKET sock = si->sock;
						void* act = si->act;


						si->pHandler->HandleAccept(sock, act);

						if(bExitThread)
						{
							ctrlGuard.Abort();
							return 0;
						}
					}
				}
				catch (...) {
					g_sysLoger.Report2(_T("Exception while HandleAccept"), EVENTLOG_ERROR_TYPE);
				}
			}
			m_acceptSocks.clear();

			for(i=0u; i<m_closedSocks.size(); ++i)
			{
				UnGuard unGuard(m_ctrlLock);
				try{
					ClosedResult& cr = m_closedSocks[i];
					SockInfo* si = cr.pSockInfo;
					if(cr.bValid)
					{
						SOCKET sock = si->sock;
						void* act = si->act;


						si->pHandler->HandleClose(sock, act);

						if(bExitThread)
						{
							ctrlGuard.Abort();
							return 0;
						}
					}
				}
				catch (...) {
					g_sysLoger.Report2(_T("Exception while HandleClose"), EVENTLOG_ERROR_TYPE);
				}
			}
			m_closedSocks.clear();

			for(i=0u; i<m_connectSocks.size(); ++i)
			{
				UnGuard unGuard(m_ctrlLock);
				try{
					ConnectResult& cr = m_connectSocks[i];
					SockInfo* si = cr.pSockInfo;
					if(cr.bValid)
					{
						SOCKET sock = si->sock;
						void* act = si->act;
						BOOL bConnected = cr.bConnected;


						si->pHandler->HandleConnect(sock, act, bConnected);

						if(bExitThread)
						{
							ctrlGuard.Abort();
							return 0;
						}
					}
				}
				catch (...) {
					g_sysLoger.Report2(_T("Exception while HandleConnect"), EVENTLOG_ERROR_TYPE);
				}
			}
			m_connectSocks.clear();
		}
	}

	return 0;
}

unsigned __stdcall CDriverThread::ThreadProc(void* pParam)
{
	CDriverThread* pDriver = (CDriverThread*)pParam;
	pDriver->HandleThreadProc();
	return 0;
}