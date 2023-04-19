/* -*- C++ -*- */

//=============================================================================
/**
*  @file    Task.h
*
*  Task.h,v 1.00 2006/01/05
*
*  @author 谢文 <xiewen@163.com>
*/
//=============================================================================

#ifndef _DBE_TASK_HEAD_
#define _DBE_TASK_HEAD_
#pragma once
#include <vector>
#include "MessageQueue.h"


namespace dbe
{
	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	class Task
	{
	public:
		typedef int (*FLUSHCALLBACK)(void* pParam, MESSAGE_TYPE msgs[], size_t nCount);
		typedef int (__stdcall *TASKCALLBACK)(void* pParam, MESSAGE_TYPE& msg);
		typedef MessageQueue<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY> MessageQueue;

		/// Constructor.
		Task (MessageQueue *pMsgQueue = NULL);
		Task (size_t hwm, size_t lwm);

		/// Destructor.
		virtual ~Task (void);

		/**
		* 打开该 Task。
		*
		* @param threadCount 希望创建的线程数量
		* @param SvcCallBack 任务回掉函数，如果为NULL，则调用Svc()方法
		* @param pParam      传给回掉函数的参数
		*
		* @retval >0 创建的线程数量
		* @retval -1 失败，如果在一个已经打开的Task上调用该方法则会失败。
		*/
		virtual size_t Open (size_t threadCount = 1, 
							 TASKCALLBACK SvcCallBack = NULL,
							 void* pParam1 = NULL,
							 FLUSHCALLBACK FlushCallBack = NULL,
							 void* pParam2 = NULL);

		/**
		* 关闭该 Task。
		*
		* @retval 关闭前Task拥有的线程数量
		*/
		virtual size_t Close (bool bTerminate = true, DWORD dwWaitTime = 1000);

		/**
		* 增加线程，Task中已有的线程数目<1的情况下调用会失败，及Task必须是
		* 已经被Open的。
		*
		* @param threadCount 希望增加的线程数量
		*
		* @retval 实际增加的线程数量
		*/
		virtual size_t AddThread(size_t threadCount = 1);

		/**
		* 减少线程，Task中已有的线程数目<=1的情况下调用会失败，及该方法不会
		* 将任务中的线程数量减少为零。
		*
		* @param threadCount 希望减少的线程数量
		*
		* @retval 实际减少的线程数量
		*/
		virtual size_t SubThread(size_t threadCount = 1);

		/// 该Task是否已经被打开
		virtual BOOL IsOpened (void);

		/// 任务处理函数，当收到任务时，由Task中的线程调度该方法，前提是调用
		/// Open()方法时SvcCallBack参数为NULL。如果返回-1，调度线程将退出。
		virtual int Svc (const MESSAGE_TYPE& msg);

		/**
		* 插入一个消息到Task中。
		*
		* @param msg     插入到Task中的消息
		* @param timeout 超时时间。如果Task中的队列已满，则会阻塞。
		*
		* @retval >0 放入该消息后Task中的消息数目。
		* @retval -1 失败，可能未放入该消息，队列已满。
		*/
		virtual int Put (MESSAGE_TYPE& msg, DWORD timeout = INFINITE);

		/**
		* 从Task尾部取出一个消息。
		*
		* @param msg     返回取出的消息
		* @param timeout 超时时间。如果Task中的队列为空，则会阻塞。
		*
		* @retval >0 取出消息后Task中的消息数目。
		* @retval -1 失败，可能队列已经为空。
		*/
		virtual int Unput (MESSAGE_TYPE& msg, DWORD timeout = INFINITE);

		/**
		* 从Task中取出一个消息
		*
		* @param msg     返回取出的消息
		* @param timeout 超时时间。如果Task中的队列为空，则会阻塞。
		*
		* @retval >0 取出消息后Task中的消息数目。
		* @retval -1 失败，可能队列已经为空。
		*/
		virtual int Get (MESSAGE_TYPE& msg, DWORD timeout = INFINITE);

		/**
		* 插入一个消息到Task头部。
		*
		* @param msg     插入Task中的消息
		* @param timeout 超时时间。如果Task中的队列已满，则会阻塞。
		*
		* @retval >0 放入该消息后Task中的消息数目。
		* @retval -1 失败，可能未放入该消息，队列已满。
		*/
		virtual int Unget (MESSAGE_TYPE& msg, DWORD timeout = INFINITE);

		/**
		* 等待Task正常关闭。
		* 
		* @param timeout 超时时间。
		*
		* @retval true  Task在超时时间内正常关闭。
		* @retval false Task还未关闭，超时。
		*/
		virtual BOOL Wait (DWORD timeout = INFINITE);

		/**
		* 挂起Task中的所有线程
		
		* @retval 挂起的线程数目。
		*/
		virtual size_t Suspend (void);

		/**
		* 继续Task中所有被挂起的线程
		*
		* @retval 继续的线程数目。
		*/
		virtual size_t Resume (void);

		/// 返回Task中的线程数目
		size_t GetThreadCount (void);

		/// 返回Task的消息队列
		MessageQueue* GetMessageQueue (void);

		/// 设置Task的消息队列，如果Task已经打开，则会设置失败。
		BOOL SetMessageQueue (MessageQueue& msgQueue);

		/// 设置标识该Task的ID
		void TaskId (int taskId);

		/// 返回标识该Task的ID
		int TaskId (void);

	protected:
		Task(const Task&){}
		Task& operator=(const Task&){}
		static unsigned __stdcall SvcRun (void* pParam);

	protected:
		typedef typename SYNCH_STRATEGY::Lock  Lock;
		typedef typename SYNCH_STRATEGY::Guard Guard;
		typedef typename SYNCH_STRATEGY::Event Event;

	protected:
		struct ThreadData
		{
			Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>* pTask;
			HANDLE	hThread;
			BOOL	bClosed;
		};

	protected:
		MessageQueue*				m_pMsgQueue;
		BOOL						m_bDeleteMsgQueue;
		std::vector<ThreadData*>	m_hThreads;
		TASKCALLBACK				TaskCallBack;
		void*						m_pCallBackParam;
		int							m_taskId;
		Lock						m_lock;
		Event						m_closeEvent;
	};
}

#include "Task.inl"

#endif