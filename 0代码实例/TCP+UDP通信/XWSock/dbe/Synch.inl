#ifndef _DBE_THREADSYNCH_CPP_
#define _DBE_THREADSYNCH_CPP_

#include "Synch.h"

namespace dbe
{
	////////////////////////////////////////////////////////////////
	// ThreadLock

	inline ThreadLock::ThreadLock(void)
	{
		m_refCount = 0;
		InitializeCriticalSection(&m_lock);
	}
	inline ThreadLock::~ThreadLock(void)
	{
		for(long i=0; i<m_refCount; i++)
			Release();
		DeleteCriticalSection(&m_lock);
	}
	inline BOOL ThreadLock::Acquire(void)
	{
		EnterCriticalSection(&m_lock); 
		m_refCount++;
		return TRUE;
	}
	inline BOOL ThreadLock::Release(void)
	{
		m_refCount--;
		LeaveCriticalSection(&m_lock); 
		return TRUE;
	}


	////////////////////////////////////////////////////////////////
	// ThreadGuard

	inline ThreadGuard::ThreadGuard(ThreadLock& lock) : m_pLock(&lock), m_bAbort(FALSE)
	{
		m_pLock->Acquire();
	}
	inline ThreadGuard::~ThreadGuard(void)
	{
		if(!m_bAbort) m_pLock->Release();
	}
	inline void ThreadGuard::Abort(void)
	{
		m_bAbort = TRUE;
	}


	////////////////////////////////////////////////////////////////
	// ThreadUnGuard

	inline ThreadUnGuard::ThreadUnGuard(ThreadLock& lock) : m_pLock(&lock)
	{
		m_pLock->Release();
	}
	inline ThreadUnGuard::~ThreadUnGuard(void)
	{
		m_pLock->Acquire();
	}


	////////////////////////////////////////////////////////////////
	// SynchEvent

	inline SynchEvent::SynchEvent(BOOL bManualReset, BOOL bInitiallyOwn,
		LPCTSTR lpszNAme, LPSECURITY_ATTRIBUTES lpsaAttribute)
	{
		m_hEvent = ::CreateEvent(lpsaAttribute, bManualReset, bInitiallyOwn, lpszNAme);
	}
	inline SynchEvent::~SynchEvent(void)
	{
		ResetEvent();
		::CloseHandle(m_hEvent);
	}
	inline BOOL SynchEvent::Wait(DWORD dwTime)
	{
		return (WaitForSingleObject(m_hEvent, dwTime) == WAIT_OBJECT_0);
	}
	inline int SynchEvent::Wait(const SynchEvent* events[], DWORD nCount, BOOL bWaitAll, DWORD dwTime)
	{
		assert(nCount <= MAXIMUM_WAIT_OBJECTS);
		HANDLE hEvents[MAXIMUM_WAIT_OBJECTS];
		for(DWORD i=0; i<nCount; i++)
		{
			hEvents[i] = events[i]->m_hEvent;
		}
		DWORD ret = WaitForMultipleObjects(nCount, hEvents, bWaitAll, dwTime);
		if(ret - WAIT_OBJECT_0 < nCount)
		{
			return int(ret - WAIT_OBJECT_0);
		}
		return -1;
	}
	inline BOOL SynchEvent::operator ! (void)
	{ 
		return m_hEvent == NULL; 
	}
	inline SynchEvent::operator HANDLE(void) 
	{ 
		return m_hEvent; 
	}
	inline BOOL SynchEvent::SetEvent(void) 
	{
		assert(m_hEvent != NULL);
		return ::SetEvent(m_hEvent);
	}
	inline BOOL SynchEvent::PulseEvent(void)
	{ 
		assert(m_hEvent != NULL); 
		return ::PulseEvent(m_hEvent); 
	}
	inline BOOL SynchEvent::ResetEvent(void)
	{ 
		assert(m_hEvent != NULL); 
		return ::ResetEvent(m_hEvent); 
	}

	////////////////////////////////////////////////////////////////
	// Semaphore

	inline SynchSemaphore::SynchSemaphore(LONG nInitialCount, LONG nMaxCount, LPCTSTR pszName, LPSECURITY_ATTRIBUTES pSecurity)
	{
		m_hSemaphore = ::CreateSemaphore( pSecurity, nInitialCount, nMaxCount, pszName );
	}
	inline SynchSemaphore::~SynchSemaphore(void)
	{
		::CloseHandle(m_hSemaphore);
	}
	inline BOOL SynchSemaphore::operator ! (void)
	{ 
		return m_hSemaphore == NULL; 
	}
	inline BOOL SynchSemaphore::Release( LONG nReleaseCount, LONG* pnOldCount)
	{
		assert( m_hSemaphore != NULL );
		return( ::ReleaseSemaphore( m_hSemaphore, nReleaseCount, pnOldCount ) );
	}
	inline BOOL SynchSemaphore::Wait(DWORD dwTime)
	{
		return (WaitForSingleObject(m_hSemaphore, dwTime) == WAIT_OBJECT_0);
	}


	////////////////////////////////////////////////////////////////
	// SynchCondition

	inline SynchCondition::SynchCondition(void)
	: m_numWaiters(0)
	, m_bBroadcast(FALSE)
	{
	}
	inline SynchCondition::~SynchCondition(void)
	{
	}
	inline BOOL SynchCondition::Signal(void)
	{
		BOOL haveWaiters;

		{
			ThreadGuard guard(m_waitersLock);
			haveWaiters = m_numWaiters > 0;
		}
		
		if (haveWaiters && !m_evCond.SetEvent())
			return FALSE;

		return TRUE;
	}
	inline BOOL SynchCondition::Broadcast(void)
	{
		BOOL haveWaiters = FALSE;
		{
			ThreadGuard guard(m_waitersLock);
			if(m_numWaiters > 0)
			{
				m_bBroadcast = TRUE;
				haveWaiters = TRUE;
			}
		}
		

		BOOL result = TRUE;
		if (haveWaiters)
		{
			// Wake up all the waiters.
			if (!m_evCond.SetEvent())
				result = FALSE;
			// Wait for all the awakened threads to acquire their part of
			// the counting event.
			if (!m_evWaitersDone.Wait())
				result = FALSE;
			// This is okay, even without the <waitersLock_> held because
			// no other waiter threads can wake up to access it.
			m_bBroadcast = FALSE;
		}
		return result;
	}
	inline BOOL SynchCondition::Wait(DWORD timeout)
	{
		{
			ThreadGuard guard(m_waitersLock);
			m_numWaiters++;
		}

		BOOL result = m_evCond.Wait(timeout);

		BOOL bLastWaiter;
		{
			ThreadGuard guard(m_waitersLock);
			m_numWaiters--;
			if(m_numWaiters > 0) m_evCond.SetEvent();
			bLastWaiter = m_bBroadcast && m_numWaiters == 0;
		}

		// If we're the last waiter thread during this particular broadcast
		// then let all the other threads proceed.
		if (bLastWaiter && !m_evWaitersDone.SetEvent())
			result = FALSE;

		return result;
	}
	inline BOOL SynchCondition::Wait(ThreadLock& unLock, DWORD timeout)
	{
		{
			ThreadGuard guard(m_waitersLock);
			m_numWaiters++;
		}

		BOOL result = TRUE;
		{
			ThreadUnGuard unGuard(unLock);

			result = m_evCond.Wait(timeout);

			BOOL bLastWaiter;
			{
				ThreadGuard guard(m_waitersLock);
				m_numWaiters--;
				if(m_numWaiters > 0) m_evCond.SetEvent();
				bLastWaiter = m_bBroadcast && m_numWaiters == 0;
			}

			// If we're the last waiter thread during this particular broadcast
			// then let all the other threads proceed.
			if (bLastWaiter && !m_evWaitersDone.SetEvent())
				result = FALSE;
		}

		return result;
	}


	////////////////////////////////////////////////////////////////
	// ThreadRWLock

	inline ThreadRWLock::ThreadRWLock(void)
	: m_refCount(0)
	, m_numWaitingReaders(0)
	, m_numWaitingWriters(0)
	{
	}
	inline ThreadRWLock::~ThreadRWLock(void)
	{
	}
	inline BOOL ThreadRWLock::AcquireWrite(DWORD timeout)
	{
		ThreadGuard guard(m_lock);

		BOOL result = TRUE;
		while (m_refCount != 0)
		{
			m_numWaitingWriters++;

			if (!m_condCanWrite.Wait(m_lock, timeout))
			{
				result = FALSE;
				break;
			}

			m_numWaitingWriters--;
		}

		m_refCount = -1;

		return result;
	}
	inline BOOL ThreadRWLock::AcquireRead(DWORD timeout)
	{
		ThreadGuard guard(m_lock);

		BOOL result = TRUE;
		// Give preference to writers who are waiting.
		while (m_refCount < 0 || m_numWaitingWriters > 0)
		{
			m_numWaitingReaders++;

			if (!m_condCanRead.Wait(m_lock, timeout))
			{
				result = FALSE;
				break;
			}
			m_numWaitingReaders--;
		}

		m_refCount++;

		return result;
	}
	inline BOOL ThreadRWLock::ReleaseRead(void)
	{
		return Release();
	}
	inline BOOL ThreadRWLock::ReleaseWrite(void)
	{
		return Release();
	}
	inline BOOL ThreadRWLock::Release(void)
	{
		ThreadGuard guard(m_lock);

		if (m_refCount > 0) // Releasing a reader.
			m_refCount--;
		else if (m_refCount == -1) // Releasing a writer.
			m_refCount = 0;
		else
			return FALSE; // @@ ASSERT ("should not be!\n");


		int result = TRUE;

		if (m_numWaitingWriters > 0 && m_refCount == 0)
			// give preference to writers over readers...
		{
			result = m_condCanWrite.Signal();
		}
		else if (m_numWaitingReaders > 0 && m_numWaitingWriters == 0)
		{
			result = m_condCanRead.Broadcast();
		}

		return result;
	}


	////////////////////////////////////////////////////////////////
	// ThreadReadGuard

	inline ThreadReadGuard::ThreadReadGuard(ThreadRWLock& lock) : m_pLock(&lock)
	{
		m_pLock->AcquireRead();
	}
	inline ThreadReadGuard::~ThreadReadGuard(void)
	{
		m_pLock->ReleaseRead();
	}


	////////////////////////////////////////////////////////////////
	// ThreadReadGuard
	inline ThreadWriteGuard::ThreadWriteGuard(ThreadRWLock& lock) : m_pLock(&lock)
	{
		m_pLock->AcquireWrite();
	}
	inline ThreadWriteGuard::~ThreadWriteGuard(void)
	{
		m_pLock->ReleaseWrite();
	}


	////////////////////////////////////////////////////////////////
	// ThreadReadUnGuard

	inline ThreadReadUnGuard::ThreadReadUnGuard(ThreadRWLock& lock) : m_pLock(&lock)
	{
		m_pLock->ReleaseRead();
	}
	inline ThreadReadUnGuard::~ThreadReadUnGuard(void)
	{
		m_pLock->AcquireRead();
	}


	////////////////////////////////////////////////////////////////
	// ThreadWriteUnGuard

	inline ThreadWriteUnGuard::ThreadWriteUnGuard(ThreadRWLock& lock) : m_pLock(&lock)
	{
		m_pLock->ReleaseWrite();
	}
	inline ThreadWriteUnGuard::~ThreadWriteUnGuard(void)
	{
		m_pLock->AcquireWrite();
	}
}

#endif