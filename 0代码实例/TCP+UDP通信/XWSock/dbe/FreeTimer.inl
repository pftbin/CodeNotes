//#include "StdAfx.h"
#include "freetimer.h"
#include <cassert>
#include <process.h>
#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace dbe
{
	inline FreeTimer::FreeTimer(void)
	: m_bInited(FALSE)
	, m_hThread(NULL)
	, m_pfn(NULL)
	, m_pParam(NULL)
	, m_startTime(0)
	, m_uDelay(INFINITE)
	, m_interval(0)
	, m_bOpened(FALSE)
	{
	}

	inline FreeTimer::~FreeTimer(void)
	{
		Uninit();
	}

	inline BOOL FreeTimer::Open(UINT uDelay, UINT interval)
	{
		if(uDelay == 0) return FALSE;
		if(!IsInited() && !Init()) return FALSE;
		{
			Guard guard(m_timerLock);

			m_startTime = ::timeGetTime();
			m_bOpened	= TRUE;
			m_uDelay	= uDelay;
			m_interval	= interval;

			m_evTimeout.SetEvent();
		}
		return TRUE;
	}

	inline void FreeTimer::Close(void)
	{
		Guard guard(m_timerLock);
		m_bOpened = FALSE;
		m_evTimeout.SetEvent();
	}

	inline void FreeTimer::SetCallback(TIMERCALLBACK pfn, void* pParam)
	{
		Guard guard(m_timerLock);
		m_pfn		= pfn;
		m_pParam	= pParam;
	}

	inline BOOL FreeTimer::Init(void)
	{
		Guard guard(m_threadLock);

		if(m_hThread == NULL)
		{
			unsigned int threadId;
			m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, &threadId);
			m_bInited = (m_hThread != NULL);
		}

		return m_bInited;
	}

	inline void FreeTimer::Uninit(void)
	{
		{
			Guard guard(m_threadLock);
			if(!m_bInited) return;
			m_bInited = FALSE;
		}
		m_evTimeout.SetEvent();
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		{
			Guard guard(m_threadLock);
			m_hThread = NULL;
		}
	}

	inline BOOL FreeTimer::IsInited(void)
	{
		return m_bInited;
	}

	inline BOOL FreeTimer::WaitTimeout(DWORD uDelay)
	{
		UnGuard unGuard(m_timerLock);
		return m_evTimeout.Wait(uDelay);
	}

	inline int FreeTimer::TimerProc(void)
	{
		Guard guard(m_timerLock);

		for(bool bExit = false; !bExit;)
		{
			DWORD uDelay = INFINITE;

			if(m_bOpened)
			{
				uDelay = (m_uDelay - (::timeGetTime()-m_startTime)%m_uDelay);
			}

			if(WaitTimeout(uDelay))
			{ // 该FreeTimer被Uninit或Close
				if(!IsInited()) bExit = true;
			}
			else
			{ // 当前等待的定时器超时
	#ifdef _DEBUG
				DWORD curTime = ::timeGetTime()-2;
	#endif
				assert(m_pfn != NULL); m_pfn(m_pParam);
	#ifdef _DEBUG
				if(m_interval > 0 && ::timeGetTime()-curTime > m_uDelay)
				{
					OutputDebugString(_T("!!!!!!!!FreeTimer超时过程处理时间大于定时间隔，将导致定时周期不正确!!!!!!!!\n"));
				}
	#endif
				if(m_interval > 0) m_uDelay = m_interval;
				else m_bOpened = FALSE;
			}
		}

		return 0;
	}

	inline unsigned __stdcall FreeTimer::ThreadProc(void* pParam)
	{
		FreeTimer* pTimer = (FreeTimer*)pParam;
		pTimer->TimerProc();
		return 0;
	}
}
