/* -*- C++ -*- */

//=============================================================================
/**
*  @file    MessageQueue.h
*
*  MessageQueue.h,v 1.00 2006/01/05
*
*  @author 谢文 <xiewen@163.com>
*/
//=============================================================================

#ifndef _DBE_MESSAGEQUEUE_HEAD_
#define _DBE_MESSAGEQUEUE_HEAD_
#pragma once
#include "Queue.h"

namespace dbe
{
	template <class TYPE>
	class DestructPointer
	{
	public:
		inline static void Release(TYPE& obj)
		{
			delete obj;
		}
	};

	template <class TYPE>
	class DestructPointers
	{
	public:
		inline static void Release(TYPE& obj)
		{
			delete [] obj;
		}
	};

	template <class TYPE>
	class DestructObject
	{
	public:
		inline static void Release(TYPE& obj)
		{
		}
	};

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	class MessageQueue
	{
	public:
		typedef int (*FLUSHCALLBACK)(void* pParam, MESSAGE_TYPE msgs[], size_t nCount);
		enum
		{
			/// Default high watermark (16 K).
			DEFAULT_HWM = 16,
			/// Default low watermark (same as high water mark).
			DEFAULT_LWM = 16,
			/// Do not use watermark
			NO_WM = -1,

			/// Message queue is active and processing normally
			ACTIVATED = 1,

			/// Queue is deactivated; no enqueue or dequeue operations allowed.
			DEACTIVATED = 2,

			/// Message queue was pulsed; enqueue and dequeue may proceed normally.
			PULSED = 3
		};

		/// Constructor
		MessageQueue(size_t hwm = DEFAULT_HWM, size_t lwm = DEFAULT_LWM);

		/// 关闭消息队列，并清除所有资源。
		virtual int Close(void);

		/// 关闭消息队列，并清除所有资源。
		virtual ~MessageQueue(void);

		/// 设置flush时的回掉函数，当MessageQueue被关闭但是队列非空时，用户可能会希望知道还有哪些消息没有处理完
		virtual int SetFlushCallBack(FLUSHCALLBACK pfn, void* pParam); 


		/**
		* 返回队列中的第一个消息，但并不移除它。
		*
		* @param firstItem 返回第一个消息
		* @param timeout   超时时间
		*
		* @retval >0 队列中的消息数目
		* @retval -1 失败，超时或队列为DEACTIVATE状态。
		*/
		virtual int PeekDequeueHead(MESSAGE_TYPE& firstItem, DWORD timeout = 0);

		/**
		* 向队列中加入一个消息。Enqueue加在尾部，EnqueueHead加在头部。
		*
		* @param newItem   要加入的消息
		* @param timeout   超时时间
		*
		* @retval >0 队列中的消息数目
		* @retval -1 失败，超时或队列为DEACTIVATE状态。
		*/
		virtual int Enqueue(MESSAGE_TYPE& newItem, DWORD timeout = 0);
		virtual int EnqueueHead(MESSAGE_TYPE& newItem, DWORD timeout = 0);

		/**
		* 从队列中取出一个消息。Dequeue从头部取，DequeueHead从尾部取。
		*
		* @param dequeued  返回取出的消息
		* @param timeout   超时时间
		*
		* @retval >0 队列中的消息数目
		* @retval -1 失败，超时或队列为DEACTIVATE状态。
		*/
		virtual int Dequeue(MESSAGE_TYPE& dequeued, DWORD timeout = 0);
		virtual int DequeueTail(MESSAGE_TYPE& dequeued, DWORD timeout = 0);

		/*
		* 清除队列中的消息
		* @retval The number of messages flushed.
		*/
		virtual int Flush(void);

		/// True if queue is full, else false.
		virtual bool IsFull(void);

		/// True if queue is empty, else false.
		virtual bool IsEmpty(void);

		/**
		* 返回高水位标
		*/
		virtual size_t HighWaterMark(void);

		/**
		* 设置高水位标，只要队列中的消息数目大于或等于该值，此时的入队操作都
		* 将被阻塞，直到消息数目低于或等于低水位标时才返回。
		*/
		virtual void HighWaterMark(size_t hwm);

		/**
		* 返回低水位标
		*/
		virtual size_t LowWaterMark(void);

		/**
		* 设置低水位标，只有当队列中的消息数目低于或等于该值时，阻塞在该队列上
		* 的入队操作才会得到通知并返回。
		*/
		virtual void LowWaterMark(size_t lwm);

		/// 返回队列中的消息数目
		virtual size_t GetMessageCount(void);

		/**
		* 将队列设置为非活动状态，并且唤醒所有阻塞在队列上的线程操作。队列中的
		* 消息不会被移除。
		*
		* @retval  队列之前的状态
		*/
		virtual int Deactivate(void);

		/**
		* 激活队列，使队列上的入队和出队操作能正常进行。
		*
		* @retval  队列之前的状态
		*/
		virtual int Activate(void);

		/**
		* Pulse the queue to wake up any waiting threads.  Changes the
		* queue state to PULSED; future enqueue/dequeue operations proceed
		* as in ACTIVATED state.
		*
		* @retval  The queue's state before this call.
		*/
		virtual int Pulse(void);

		/// 返回队列的当前状态
		virtual int GetState(void);

		/// Returns true if the state of the queue is DEACTIVATED,
		/// and false if the queue's state is ACTIVATED or PULSED.
		virtual bool IsDeactivated(void);

	protected:

		/// Release all resources from the message queue but do not mark it
		/// as deactivated.
		/**
		* The caller must be holding the queue lock before calling this
		* method.
		*
		* @return The number of messages flushed.
		*/
		virtual int Flush_i (void);

		/// Enqueue a message at the tail of the queue.
		virtual int Enqueue_i (MESSAGE_TYPE& newItem);

		/// Enqueue a message at the head of the queue.
		virtual int EnqueueHead_i (MESSAGE_TYPE& newItem);

		/// Dequeue and return the message at the head of the
		/// queue.
		virtual int Dequeue_i (MESSAGE_TYPE& dequeued);

		/// Dequeue and return the message at the tail of the
		/// queue.
		virtual int DequeueTail_i (MESSAGE_TYPE& dequeued);

		/// True if queue is full, else false.
		virtual bool IsFull_i (void);

		/// True if queue is empty, else false.
		virtual bool IsEmpty_i (void);

		// = Implementation of the public <activate> and <deactivate> methods.

		// These methods assume locks are held.

		/**
		* Notifies all waiting threads that the queue has been deactivated
		* so they can wakeup and continue other processing.
		* No messages are removed from the queue.
		*
		* @param pulse  If 0, the queue's state is changed to DEACTIVATED
		*               and any other operations called until the queue is
		*               reactivated will immediately return -1.
		*               If not zero, only the waiting threads are notified and
		*               the queue's state changes to PULSED.
		*
		* @return The state of the queue before the call.
		*/
		virtual int Deactivate_i (int pulse = 0);

		/// Activate the queue.
		virtual int Activate_i (void);

		/// Wait for the queue to become non-full.
		virtual int WaitNotFull (DWORD timeout);

		/// Wait for the queue to become non-empty.
		virtual int WaitNotEmpty (DWORD timeout);

		/// Inform any threads waiting to enqueue that they can procede.
		virtual int SignalEnqueueWaiters (void);

		/// Inform any threads waiting to dequeue that they can procede.
		virtual int SignalDequeueWaiters (void);

	protected:
		typedef typename SYNCH_STRATEGY::Lock  Lock;
		typedef typename SYNCH_STRATEGY::Guard Guard;
		typedef typename SYNCH_STRATEGY::Condition Condition;
		typedef QueueAsArray<MESSAGE_TYPE> Queue;

	protected:
		/// Indicates the state of the queue, which can be
		/// <ACTIVATED>, <DEACTIVATED>, or <PULSED>.
		int			m_state;

		Queue		m_queue;
		Lock		m_lock;
		Condition	m_notEmptyCond;
		Condition	m_notFullCond;
		size_t		m_highWaterMark;
		size_t		m_lowWaterMark;
		FLUSHCALLBACK m_flushCallBack;
		void*		m_pFuncParam;
	};
}

#include "MessageQueue.inl"

#endif