/* -*- C++ -*- */

//=============================================================================
/**
*  @file    FreeTimer.h
*
*  FreeTimer.h,v 1.00 2006/01/18
*
*  @author л�� <xiewen@163.com>
*
*  FreeTimer���Լ�����������һ���̣߳�����ʵ�ֶ�ʱ���ԣ�����ÿ��һ��FreeTimer
*  �ͻ�����һ���̣߳����ǣ�ֻҪ��ʱ������̵�ִ��ʱ��С�ڶ�ʱ���ڣ�FreeTimer��
*  ��ʱ����ǳ�׼ȷ������ʹ�÷ǳ����㣬ֻ��Ҫ����һ��FreeTimer�������ó�ʱ��
*  ������������Էǳ���Ч�Ĵ򿪺͹رն�ʱ��������Ҫ����������ʱ����Ӧ�ó����У�
*  FreeTimer�Ƿǳ����õģ�
*/
//=============================================================================

#ifndef _DBE_FREETIMER_HEAD_
#define _DBE_FREETIMER_HEAD_
#pragma once
#include "Synch.h"

typedef void*	HTIMER;
typedef int (__stdcall *TIMERCALLBACK)(void* pParam);

namespace dbe
{
	class FreeTimer
	{
	public:
		FreeTimer(void);
		~FreeTimer(void);

		/// ��ʼ����ʱ����������������ʼ��ʱ���̣߳�
		virtual BOOL Init(void);

		/// ж�ض�ʱ��������������ʱ���̣߳���ʱ����ֹͣ������
		virtual void Uninit(void);

		/// �����Ƿ񱻳�ʼ����
		virtual BOOL IsInited(void);

		/**
		* �򿪶�ʱ���������δ����ʼ���������ȳ�ʼ����ʱ����
		*
		* @param uDelay      ��uDelay������һ�ε���
		* @param interval    ��ʱ�������ڣ����Ϊ�����һ�ε��ں�ʱ���Զ��ر�
		*
		* @retval TRUE       �ɹ�
		* @retval FALSE      ʧ��
		*/
		virtual BOOL Open(UINT uDelay, UINT interval = 0);

		/// �رն�ʱ����
		virtual void Close(void);

		/**
		* ���ó�ʱ�ص�����������ȷ���ڴ򿪶�ʱ��֮ǰ���ø÷�����
		*
		* @param pfn         ��ʱ�ص�����
		* @param pParam      ������ʱ�ص������Ĳ���
		*
		* @retval            �޷���ֵ
		*/
		void SetCallback(TIMERCALLBACK pfn, void* pParam);

	protected:
		virtual int TimerProc(void);
		BOOL WaitTimeout(DWORD uDelay);
		static unsigned __stdcall ThreadProc(void* pParam);

	protected:
		typedef SynchEvent				Event;
		typedef ThreadLock				Lock;
		typedef ThreadGuard				Guard;
		typedef ThreadUnGuard			UnGuard;

	protected:
		TIMERCALLBACK		m_pfn;
		void*				m_pParam;
		DWORD				m_startTime;
		UINT				m_uDelay;
		UINT				m_interval;
		BOOL				m_bOpened;

		Lock				m_threadLock;
		Lock				m_timerLock;
		Event				m_evTimeout;
		HANDLE				m_hThread;
		BOOL				m_bInited;
	};
}

#include "FreeTimer.inl"

#endif
