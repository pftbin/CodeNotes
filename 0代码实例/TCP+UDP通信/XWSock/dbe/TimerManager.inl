//#include "StdAfx.h"
#include "timermanager.h"
#include <algorithm>
#include <cassert>
#include <process.h>
#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace dbe
{
	////////////////////////////////////////////////////////////////////////
	// class TimerNode
	inline TimerManager::TimerNode::TimerNode(void)
	: m_pfn(NULL)
	, m_pParam(NULL)
	, m_firstTime(0)
	, m_interval(0)
	, m_bOpened(FALSE)
	, m_pTimer(NULL)
	, m_serial(0)
	{
	}
	inline void TimerManager::TimerNode::SetCallback(TIMERCALLBACK pfn, void* pParam)
	{
		m_pfn = pfn;
		m_pParam = pParam;
	}

	inline void TimerManager::TimerNode::Open(DWORD curTime, UINT uDelay, UINT uPrecision, UINT interval)
	{
		m_pTimer = this;
		m_firstTime = curTime + uDelay;
		m_precision = uPrecision;
		m_interval = interval;
		m_bOpened = TRUE;
		++m_serial;
	}

	inline void TimerManager::TimerNode::Close(void)
	{
		m_pTimer = NULL;
		m_bOpened = FALSE;
	}

	inline BOOL TimerManager::TimerNode::IsOpened(void) const
	{
		return m_bOpened;
	}

	inline UINT TimerManager::TimerNode::GetPrecision(void) const
	{
		return m_precision;
	}

	inline UINT TimerManager::TimerNode::GetInterval(void) const
	{
		return m_interval;
	}

	inline DWORD TimerManager::TimerNode::GetSerial(void) const
	{
		return m_serial;
	}

	inline DWORD TimerManager::TimerNode::GetFirstTimeoutTime(void) const
	{
		return m_firstTime;
	}

	//DWORD TimerManager::TimerNode::GetPrevTimeoutTime(void) const
	//{
	//	return m_prevTimeout;
	//}

	inline DWORD TimerManager::TimerNode::GetNextTimeoutTime(DWORD curTime) const
	{
		if(m_interval == 0) return m_firstTime;
		return curTime + (m_interval - (curTime-m_firstTime)%m_interval);
	}
	inline int TimerManager::TimerNode::Expire(void)
	{
		return m_pfn(m_pParam);
	}

	////////////////////////////////////////////////////////////////////////
	// class TimerRef
	inline TimerManager::TimerRef::TimerRef(void)
	: m_ppTimer(NULL)
	{
	}
	inline TimerManager::TimerRef::TimerRef(TimerNode* pTimer)
	{
		SetTimerNode(pTimer);
	}
	inline DWORD TimerManager::TimerRef::GetSerial(void) const
	{
		return m_serial;
	}
	inline TimerManager::TimerNode* TimerManager::TimerRef::GetTimerNode(void) const
	{
		return m_ppTimer == NULL ? NULL : *m_ppTimer;
	}
	inline void TimerManager::TimerRef::SetTimerNode(TimerNode* pTimer)
	{
		if(pTimer != NULL)
		{
			m_ppTimer = &pTimer->m_pTimer;
			m_serial = pTimer->m_serial;
		}
		else
		{
			m_ppTimer = NULL;
			m_serial = 0;
		}
	}

	////////////////////////////////////////////////////////////////////////
	// class TimeoutInfo
	inline TimerManager::TimeoutInfo::TimeoutInfo(void)
	: m_timeTimeout(0)
	{
	}
	inline TimerManager::TimeoutInfo::TimeoutInfo(TimerNode* pTimer, DWORD timeTimeout)
	: TimerRef(pTimer)
	, m_timeTimeout(timeTimeout)
	{
	}
	inline bool TimerManager::TimeoutInfo::operator < (const TimeoutInfo& other) const
	{
		return m_timeTimeout < other.m_timeTimeout;
	}
	inline void TimerManager::TimeoutInfo::SetTimeTimeout(DWORD timeTimeout)
	{
		m_timeTimeout = timeTimeout;
	}
	inline DWORD TimerManager::TimeoutInfo::GetTimeTimeout(void) const
	{
		return m_timeTimeout;
	}
	inline UINT TimerManager::TimeoutInfo::GetIntervalToTimeout(DWORD curTime) const
	{
		return UINT(curTime > m_timeTimeout ? 0 : m_timeTimeout-curTime);
	}
	inline UINT TimerManager::TimeoutInfo::GetMaxIntervalToTimeout(DWORD curTime, UINT uPrecision) const
	{
		return UINT(curTime > m_timeTimeout+uPrecision ? 0 : m_timeTimeout+uPrecision-curTime);
	}

	//////////////////////////////////////////////////////////////////////////
	// class TimerQueueVisitor
	inline TimerManager::TimerQueueVisitor::TimerQueueVisitor(TimerNode* pTimer)
	: m_pTimer(pTimer)
	{
	}
	inline void TimerManager::TimerQueueVisitor::operator () (const TimeoutInfo& ti)
	{
		if(m_pTimer != NULL && ti.GetTimerNode() == m_pTimer)
		{
			TimeoutInfo& info = const_cast<TimeoutInfo&>(ti);
			info.SetTimerNode(NULL);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// class TimerManager
	inline TimerManager::TimerManager(void)
	: m_bOpened(FALSE)
	, m_hThread(NULL)
	{
	}

	inline TimerManager::~TimerManager(void)
	{
		Close();
		ClearTimer();
	}

	inline HTIMER TimerManager::CreateTimer(TIMERCALLBACK pfn, void* pParam)
	{
		TimerNode* pTimer = new TimerNode();
		pTimer->SetCallback(pfn, pParam);
		{
			Guard guard(m_timerLock);
			m_allTimers.insert(pTimer);
		}
		return (HTIMER)pTimer;
	}

	inline void TimerManager::DeleteTimer(HTIMER hTimer)
	{
		TimerNode* pTimer = (TimerNode*)hTimer;
		{
			Guard guard(m_timerLock);

			TimerSet::iterator pos = m_allTimers.find(pTimer);
			if(pos != m_allTimers.end())
			{
				TimerQueueVisitor visitor(pTimer);
				m_timerQueue.accept(visitor);
				CloseTimer_i(pTimer);
				m_allTimers.erase(pos);
				delete pTimer;
			}
		}
	}

	inline void TimerManager::ClearTimer(void)
	{
		Guard guard(m_timerLock);
		TimerSet::iterator pos = m_allTimers.begin();
		for(; pos != m_allTimers.end(); ++pos)
		{
			delete *pos;
		}
		m_allTimers.clear();
		m_timerQueue.clear();
		m_evTimeout.SetEvent();
	}

	inline BOOL TimerManager::OpenTimer(HTIMER hTimer, UINT uDelay, UINT uPrecision, UINT interval)
	{
		TimerNode* pTimer = (TimerNode*)hTimer;
		{
			Guard guard(m_timerLock);
			assert(m_allTimers.find(pTimer) != m_allTimers.end());
			return OpenTimer_i(pTimer, uDelay, uPrecision, interval);
		}
		return FALSE;
	}

	inline void TimerManager::CloseTimer(HTIMER hTimer)
	{
		TimerNode* pTimer = (TimerNode*)hTimer;
		{
			Guard guard(m_timerLock);
			assert(m_allTimers.find(pTimer) != m_allTimers.end());
			CloseTimer_i(pTimer);
		}
	}

	inline BOOL TimerManager::OpenTimer_i(TimerNode* pTimer, UINT uDelay, UINT uPrecision, UINT interval)
	{
		if(pTimer->IsOpened()) return TRUE;

		pTimer->Open(::timeGetTime(), uDelay, uPrecision, interval);
		m_timerQueue.push(TimeoutInfo(pTimer, pTimer->GetFirstTimeoutTime()));

		if(m_timerQueue.top().GetTimerNode() == pTimer)
		{
			m_evTimeout.SetEvent();
		}

		return TRUE;
	}

	inline void TimerManager::CloseTimer_i(TimerNode* pTimer)
	{
		if(!pTimer->IsOpened()) return;

		pTimer->Close();

		TimeoutInfo top;

		if(!m_timerQueue.empty())
		{
			top = m_timerQueue.top();
		}

		if(top.GetTimerNode() == pTimer)
		{
			m_evTimeout.SetEvent();
		}
	}

	inline BOOL TimerManager::Open(void)
	{
		Guard guard(m_mgrLock);

		if(m_hThread == NULL)
		{
			unsigned int threadId;
			m_hThread = (HANDLE)_beginthreadex(NULL, 0, TimerMonitorProc, this, 0, &threadId);
			m_bOpened = (m_hThread != NULL);
		}

		return m_bOpened;
	}

	inline void TimerManager::Close(void)
	{
		{
			Guard guard(m_mgrLock);
			if(!m_bOpened) return;
			m_bOpened = FALSE;
		}
		{
			Guard guard(m_timerLock);
			m_evTimeout.SetEvent();
		}
		
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		{
			Guard guard(m_mgrLock);
			m_hThread = NULL;
		}
	}

	inline BOOL TimerManager::IsOpened(void)
	{
		return m_bOpened;
	}

	inline BOOL TimerManager::WaitTimeout(DWORD uDelay)
	{
		UnGuard unGuard(m_timerLock);
		return m_evTimeout.Wait(uDelay);
	}

	inline unsigned __stdcall TimerManager::TimerMonitorProc(void* pParam)
	{
		TimerManager* pMgr = (TimerManager*)pParam;
		pMgr->TimerProc();
		return 0;
	}

	inline int TimerManager::TimerProc(void)
	{
		Guard guard(m_timerLock);

		while(true)
		{
			UINT uDelay = 0;
			while(!m_timerQueue.empty() && uDelay == 0 && IsOpened())
			{
				const TimeoutInfo& top = m_timerQueue.top();
				TimerNode* pTimer = top.GetTimerNode();
				if(pTimer != NULL && pTimer->GetSerial() == top.GetSerial())
				{
					uDelay = top.GetIntervalToTimeout(::timeGetTime());
					if(uDelay == 0)
					{
//#ifdef _DEBUG
//						if(top.GetMaxIntervalToTimeout(curTime, pTimer->GetPrecision()) == 0)
//						{
//							OutputDebugString("!!!!!!!!TimerManager中的Timer过多或处理过程时间太长，将导致定时周期不正确!!!!!!!!\n");
//						}
//#endif
						m_timerQueue.pop();
						pTimer->Expire();
						if(pTimer->GetInterval() > 0)
						{
							DWORD curTime = ::timeGetTime();
							if(curTime < top.GetTimeTimeout())
							{
								curTime = top.GetTimeTimeout()+1;
							}
							m_timerQueue.push(TimeoutInfo(pTimer, pTimer->GetNextTimeoutTime(curTime)));
						}
						else
						{
							pTimer->Close();
						}
					}
				}
				else
				{
					m_timerQueue.pop();
				}

				m_evTimeout.ResetEvent();

				{
					UnGuard unGuard(m_timerLock);
				}
			}

			if(uDelay == 0) uDelay = INFINITE;

			if(WaitTimeout(uDelay))
			{ // 当前等待的定时器被关闭或TimerManager被关闭
				if(!IsOpened())
					break;
			}
			else
			{ // 当前等待的定时器超时
				if(!m_timerQueue.empty())
				{
					const TimeoutInfo& top = m_timerQueue.top();
					TimerNode* pTimer = const_cast<TimerNode*>(top.GetTimerNode());
					if(pTimer != NULL && pTimer->GetSerial() == top.GetSerial())
					{
						m_timerQueue.pop();
						pTimer->Expire();
						if(pTimer->GetInterval() > 0)
						{
							DWORD curTime = ::timeGetTime();
							if(curTime < top.GetTimeTimeout())
							{
								curTime = top.GetTimeTimeout()+1;
							}
							m_timerQueue.push(TimeoutInfo(pTimer, pTimer->GetNextTimeoutTime(curTime)));
						}
						else
						{
							pTimer->Close();
						}
					}
					else
					{
						m_timerQueue.pop();
					}
				}
			}

			{
				UnGuard unGuard(m_timerLock);
			}
		}

		return 0;
	}
}