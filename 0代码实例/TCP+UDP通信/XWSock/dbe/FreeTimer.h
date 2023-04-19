/* -*- C++ -*- */

//=============================================================================
/**
*  @file    FreeTimer.h
*
*  FreeTimer.h,v 1.00 2006/01/18
*
*  @author 谢文 <xiewen@163.com>
*
*  FreeTimer会自己创建并管理一个线程，用于实现定时策略，所以每打开一个FreeTimer
*  就会增加一个线程．但是，只要超时处理过程的执行时间小于定时周期，FreeTimer的
*  定时都会非常准确，而且使用非常方便，只需要创建一个FreeTimer对象，设置超时回
*  掉函数，便可以非常高效的打开和关闭定时器．在需要设置少量定时器的应用程序中，
*  FreeTimer是非常适用的．
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

		/// 初始化定时器，它将创建并开始定时器线程．
		virtual BOOL Init(void);

		/// 卸载定时器，它将结束定时器线程，定时器将停止工作．
		virtual void Uninit(void);

		/// 返回是否被初始化．
		virtual BOOL IsInited(void);

		/**
		* 打开定时器，如果还未被初始化，它将先初始化定时器．
		*
		* @param uDelay      于uDelay毫秒后第一次到期
		* @param interval    定时器的周期，如果为０则第一次到期后定时器自动关闭
		*
		* @retval TRUE       成功
		* @retval FALSE      失败
		*/
		virtual BOOL Open(UINT uDelay, UINT interval = 0);

		/// 关闭定时器．
		virtual void Close(void);

		/**
		* 设置超时回掉函数，必须确保在打开定时器之前调用该方法．
		*
		* @param pfn         超时回掉函数
		* @param pParam      传给超时回掉函数的参数
		*
		* @retval            无返回值
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
