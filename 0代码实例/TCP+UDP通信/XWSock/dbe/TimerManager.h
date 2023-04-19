/* -*- C++ -*- */

//=============================================================================
/**
*  @file    TimerManager.h
*
*  TimerManager.h,v 1.00 2006/01/05
*
*  @author 谢文 <xiewen@163.com>
*
*  TimerManager可以创建和管理多个定时器，这些定时器都可以高效的打开和关闭．每个
*  TimerManager创建和维护一个线程，并在该线程中实现所有定时器的定时策略．在需要
*  大量定时器的应用程序中，通过TimerManager来设置定时器是非常适合的．但是，有一
*  点需要注意的是，所有定时器的超时处理过程都应该立刻返回，不能阻塞．因为所有定
*  时器的定时和处理都在同一个线程中，所以一旦有一个阻塞或执行时间过长都将影响其
*  他定时器的准确定时．如果超时处理过程需要执行较长时间，可以考虑配合使用Task框
*  架来完成，使超时过程立刻返回．
*/
//=============================================================================

#ifndef _DBE_TIMERMANAGER_HEAD_
#define _DBE_TIMERMANAGER_HEAD_
#pragma once
#include <set>
#include "Synch.h"
#include "Task.h"
#include "PriorityQueue.h"

typedef void*	HTIMER;
typedef int (__stdcall *TIMERCALLBACK)(void* pParam);

namespace dbe
{
	class TimerManager
	{
	public:
		TimerManager(void);
		~TimerManager(void);

		/// 打开TimerManager，它将创建并开始定时器线程，成功之后所有
		/// 创建的定时器才能正常运行．
		BOOL Open(void);

		/// 关闭TimerManager，它将结束定时器线程，所有创建的定时器都
		/// 将停止工作．
		void Close(void);

		/// 返回TimerManager是否已经打开
		BOOL IsOpened(void);

		/**
		* 创建一个定时器．
		*
		* @param pfn         超时回掉函数
		* @param pParam      传给超时回掉函数的参数
		*
		* @retval != NULL    定时器的句柄
		* @retval NULL       失败
		*/
		HTIMER CreateTimer(TIMERCALLBACK pfn, void* pParam);

		/**
		* 删除定时器，如果删除前未关闭，它将负责关闭该定时器．
		*
		* @param hTimer      要删除的定时器句柄
		*
		* @retval            无返回值
		*/
		void DeleteTimer(HTIMER hTimer);

		/// 删除所有的定时器．
		void ClearTimer(void);

		/**
		* 打开定时器．
		*
		* @param hTimer      定时器句柄
		* @param uDelay      于uDelay毫秒后第一次到期
		* @param uPrecision  定时器的精度（目前不支持）
		* @param interval    定时器的周期，如果为０则第一次到期后定时器自动关闭
		*
		* @retval TRUE       成功
		* @retval FALSE      失败
		*/
		BOOL OpenTimer(HTIMER hTimer, UINT uDelay, UINT uPrecision, UINT interval = 0);

		/**
		* 关闭定时器，该方法返回后将确保定时器的超时过程不会正在执行或再被执行．
		*
		* @param hTimer      要删除的定时器句柄
		*
		* @retval            无返回值
		*/
		void CloseTimer(HTIMER hTimer);

	protected:
		class TimerRef;
		class TimerNode
		{
		public:
			TimerNode(void);
			void SetCallback(TIMERCALLBACK pfn, void* pParam);
			void Open(DWORD curTime, UINT uDelay, UINT uPrecision, UINT interval);
			void Close(void);
			BOOL IsOpened(void) const;
			UINT GetInterval(void) const;
			UINT GetPrecision(void) const;
			DWORD GetSerial(void) const;
			DWORD GetFirstTimeoutTime(void) const;
			//DWORD GetPrevTimeoutTime(void) const;
			DWORD GetNextTimeoutTime(DWORD curTime) const;
			int Expire(void);
		private:
			TIMERCALLBACK	m_pfn;
			void*			m_pParam;
			DWORD			m_firstTime;
			UINT			m_precision;
			UINT			m_interval;
			BOOL			m_bOpened;
			DWORD			m_serial;
			TimerNode*		m_pTimer;
			friend class TimerRef;
		};

		class TimerRef
		{
		public:
			TimerRef(void);
			TimerRef(TimerNode* pTimer);
			DWORD GetSerial(void) const;
			TimerNode* GetTimerNode(void) const;
			void SetTimerNode(TimerNode* pTimer);
		protected:
			TimerNode**		m_ppTimer;
			DWORD			m_serial;
		};

		class TimeoutInfo : public TimerRef
		{
		public:
			TimeoutInfo(void);
			TimeoutInfo(TimerNode* pTimer, DWORD timeTimeout);
			bool operator < (const TimeoutInfo& other) const;
			void SetTimeTimeout(DWORD timeTimeout);
			DWORD GetTimeTimeout(void) const;
			UINT GetIntervalToTimeout(DWORD curTime) const;
			UINT GetMaxIntervalToTimeout(DWORD curTime, UINT uPrecision) const;
		private:
			DWORD			m_timeTimeout;
		};

		class TimerQueueVisitor
		{
		public:
			TimerQueueVisitor(TimerNode* pTimer);
			void operator () (const TimeoutInfo& ti);
		private:
			TimerNode* m_pTimer;
		};

	protected:
		BOOL OpenTimer_i(TimerNode* pTimer, UINT uDelay, UINT uPrecision, UINT interval = 0);
		void CloseTimer_i(TimerNode* pTimer);
		BOOL WaitTimeout(DWORD uDelay);
		virtual int TimerProc(void);
		static unsigned __stdcall TimerMonitorProc(void* pParam);
		
	protected:
		typedef SynchEvent				Event;
		typedef ThreadLock				Lock;
		typedef ThreadGuard				Guard;
		typedef ThreadUnGuard			UnGuard;
		
		typedef std::set<TimerNode*>	TimerSet;
		typedef PriorityQueue<TimeoutInfo, TimerQueueVisitor>	TimerQueue;

	protected:
		TimerSet						m_allTimers;
		TimerQueue						m_timerQueue;

		Lock							m_mgrLock;
		Lock							m_timerLock;
		Event							m_evTimeout;
		HANDLE							m_hThread;
		BOOL							m_bOpened;
	};
}

#include "TimerManager.inl"

#endif
