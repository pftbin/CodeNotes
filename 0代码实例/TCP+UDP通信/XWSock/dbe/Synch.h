/* -*- C++ -*- */

//=============================================================================
/**
*  @file    Synch.h
*
*  Synch.h,v 1.00 2006/01/05
*
*  @author п╩нд <xiewen@163.com>
*/
//=============================================================================

#ifndef _DBE_THREADSYNCH_HEAD_
#define _DBE_THREADSYNCH_HEAD_
#pragma once
#include <cassert>

namespace dbe
{
	////////////////////////////////////////////////////////////////
	// ThreadLock
	class ThreadLock
	{
	public:
		ThreadLock(void);
		~ThreadLock(void);
		BOOL Acquire(void);
		BOOL Release(void);
	private:
		long m_refCount;
		CRITICAL_SECTION m_lock;
	};

	////////////////////////////////////////////////////////////////
	// ThreadGuard
	class ThreadGuard
	{
	public:
		ThreadGuard(ThreadLock& lock);
		~ThreadGuard(void);
		void Abort(void);
	private:
		BOOL m_bAbort;
		ThreadLock* m_pLock;
	};

	////////////////////////////////////////////////////////////////
	// ThreadUnGuard
	class ThreadUnGuard
	{
	public:
		ThreadUnGuard(ThreadLock& lock);
		~ThreadUnGuard(void);
	private:
		ThreadLock* m_pLock;
	};

	////////////////////////////////////////////////////////////////
	// SynchEvent
	class SynchEvent
	{
	public:
		SynchEvent(BOOL bManualReset = FALSE, BOOL bInitiallyOwn = FALSE,
				LPCTSTR lpszNAme = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
		~SynchEvent(void);
		BOOL Wait(DWORD dwTime = INFINITE);
		static int Wait(const SynchEvent* events[], DWORD nCount, BOOL bWaitAll = FALSE, DWORD dwTime = INFINITE);
		BOOL operator ! (void);
		operator HANDLE(void);
		BOOL SetEvent(void);
		BOOL PulseEvent(void);
		BOOL ResetEvent(void);
	private:
		HANDLE m_hEvent;
	};

	////////////////////////////////////////////////////////////////
	// Semaphore
	class SynchSemaphore
	{
	public:
		SynchSemaphore(LONG nInitialCount, LONG nMaxCount, LPCTSTR pszName = NULL, LPSECURITY_ATTRIBUTES pSecurity = NULL);
		~SynchSemaphore(void);
		BOOL operator ! (void);
		BOOL Release( LONG nReleaseCount = 1, LONG* pnOldCount = NULL);
		BOOL Wait(DWORD dwTime = INFINITE);
	private:
		HANDLE	m_hSemaphore;
	};

	////////////////////////////////////////////////////////////////
	// SynchCondition
	class SynchCondition
	{
	public:
		SynchCondition(void);
		~SynchCondition(void);
		BOOL Signal(void);
		BOOL Broadcast(void);
		BOOL Wait(DWORD timeout = INFINITE);
		BOOL Wait(ThreadLock& unLock, DWORD timeout = INFINITE);
	private:
		BOOL m_bBroadcast;
		long m_numWaiters;
		ThreadLock m_waitersLock;
		SynchEvent m_evCond;
		SynchEvent m_evWaitersDone;
	};

	////////////////////////////////////////////////////////////////
	// ThreadRWLock
	class ThreadRWLock
	{
	public:
		ThreadRWLock(void);
		~ThreadRWLock(void);
		BOOL AcquireWrite(DWORD timeout = INFINITE);
		BOOL AcquireRead(DWORD timeout = INFINITE);
		BOOL ReleaseRead(void);
		BOOL ReleaseWrite(void);
	private:
		BOOL Release(void);
	private:
		ThreadLock			m_lock;
		SynchCondition		m_condCanWrite;
		SynchCondition		m_condCanRead;
		int					m_refCount;
		int					m_numWaitingReaders;
		int					m_numWaitingWriters;
	};

	////////////////////////////////////////////////////////////////
	// ThreadReadGuard
	class ThreadReadGuard
	{
	public:
		ThreadReadGuard(ThreadRWLock& lock);
		~ThreadReadGuard(void);
	private:
		ThreadRWLock* m_pLock;
	};

	////////////////////////////////////////////////////////////////
	// ThreadReadGuard
	class ThreadWriteGuard
	{
	public:
		ThreadWriteGuard(ThreadRWLock& lock);
		~ThreadWriteGuard(void);
	private:
		ThreadRWLock* m_pLock;
	};

	////////////////////////////////////////////////////////////////
	// ThreadReadUnGuard
	class ThreadReadUnGuard
	{
	public:
		ThreadReadUnGuard(ThreadRWLock& lock);
		~ThreadReadUnGuard(void);
	private:
		ThreadRWLock* m_pLock;
	};

	////////////////////////////////////////////////////////////////
	// ThreadWriteUnGuard
	class ThreadWriteUnGuard
	{
	public:
		ThreadWriteUnGuard(ThreadRWLock& lock);
		~ThreadWriteUnGuard(void);
	private:
		ThreadRWLock* m_pLock;
	};

	////////////////////////////////////////////////////////////////
	// ThreadSynch
	class ThreadSynch
	{
	public:
		typedef ThreadLock			Lock;
		typedef ThreadRWLock		RWLock;

		typedef ThreadGuard			Guard;
		typedef ThreadUnGuard		UnGuard;
		typedef ThreadReadGuard		ReadGuard;
		typedef ThreadWriteGuard	WriteGuard;
		typedef ThreadReadUnGuard	ReadUnGuard;
		typedef ThreadWriteUnGuard	WriteUnGuard;

		typedef SynchEvent			Event;
		typedef SynchSemaphore		Semaphore;
		typedef SynchCondition		Condition;
	};
}

#include "Synch.inl"

#endif