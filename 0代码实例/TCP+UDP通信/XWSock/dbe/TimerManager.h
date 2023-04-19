/* -*- C++ -*- */

//=============================================================================
/**
*  @file    TimerManager.h
*
*  TimerManager.h,v 1.00 2006/01/05
*
*  @author л�� <xiewen@163.com>
*
*  TimerManager���Դ����͹�������ʱ������Щ��ʱ�������Ը�Ч�Ĵ򿪺͹رգ�ÿ��
*  TimerManager������ά��һ���̣߳����ڸ��߳���ʵ�����ж�ʱ���Ķ�ʱ���ԣ�����Ҫ
*  ������ʱ����Ӧ�ó����У�ͨ��TimerManager�����ö�ʱ���Ƿǳ��ʺϵģ����ǣ���һ
*  ����Ҫע����ǣ����ж�ʱ���ĳ�ʱ������̶�Ӧ�����̷��أ�������������Ϊ���ж�
*  ʱ���Ķ�ʱ�ʹ�����ͬһ���߳��У�����һ����һ��������ִ��ʱ���������Ӱ����
*  ����ʱ����׼ȷ��ʱ�������ʱ���������Ҫִ�нϳ�ʱ�䣬���Կ������ʹ��Task��
*  ������ɣ�ʹ��ʱ�������̷��أ�
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

		/// ��TimerManager��������������ʼ��ʱ���̣߳��ɹ�֮������
		/// �����Ķ�ʱ�������������У�
		BOOL Open(void);

		/// �ر�TimerManager������������ʱ���̣߳����д����Ķ�ʱ����
		/// ��ֹͣ������
		void Close(void);

		/// ����TimerManager�Ƿ��Ѿ���
		BOOL IsOpened(void);

		/**
		* ����һ����ʱ����
		*
		* @param pfn         ��ʱ�ص�����
		* @param pParam      ������ʱ�ص������Ĳ���
		*
		* @retval != NULL    ��ʱ���ľ��
		* @retval NULL       ʧ��
		*/
		HTIMER CreateTimer(TIMERCALLBACK pfn, void* pParam);

		/**
		* ɾ����ʱ�������ɾ��ǰδ�رգ���������رոö�ʱ����
		*
		* @param hTimer      Ҫɾ���Ķ�ʱ�����
		*
		* @retval            �޷���ֵ
		*/
		void DeleteTimer(HTIMER hTimer);

		/// ɾ�����еĶ�ʱ����
		void ClearTimer(void);

		/**
		* �򿪶�ʱ����
		*
		* @param hTimer      ��ʱ�����
		* @param uDelay      ��uDelay������һ�ε���
		* @param uPrecision  ��ʱ���ľ��ȣ�Ŀǰ��֧�֣�
		* @param interval    ��ʱ�������ڣ����Ϊ�����һ�ε��ں�ʱ���Զ��ر�
		*
		* @retval TRUE       �ɹ�
		* @retval FALSE      ʧ��
		*/
		BOOL OpenTimer(HTIMER hTimer, UINT uDelay, UINT uPrecision, UINT interval = 0);

		/**
		* �رն�ʱ�����÷������غ�ȷ����ʱ���ĳ�ʱ���̲�������ִ�л��ٱ�ִ�У�
		*
		* @param hTimer      Ҫɾ���Ķ�ʱ�����
		*
		* @retval            �޷���ֵ
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
