/* -*- C++ -*- */

//=============================================================================
/**
*  @file    MessageQueue.h
*
*  MessageQueue.h,v 1.00 2006/01/05
*
*  @author л�� <xiewen@163.com>
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

		/// �ر���Ϣ���У������������Դ��
		virtual int Close(void);

		/// �ر���Ϣ���У������������Դ��
		virtual ~MessageQueue(void);

		/// ����flushʱ�Ļص���������MessageQueue���رյ��Ƕ��зǿ�ʱ���û����ܻ�ϣ��֪��������Щ��Ϣû�д�����
		virtual int SetFlushCallBack(FLUSHCALLBACK pfn, void* pParam); 


		/**
		* ���ض����еĵ�һ����Ϣ���������Ƴ�����
		*
		* @param firstItem ���ص�һ����Ϣ
		* @param timeout   ��ʱʱ��
		*
		* @retval >0 �����е���Ϣ��Ŀ
		* @retval -1 ʧ�ܣ���ʱ�����ΪDEACTIVATE״̬��
		*/
		virtual int PeekDequeueHead(MESSAGE_TYPE& firstItem, DWORD timeout = 0);

		/**
		* ������м���һ����Ϣ��Enqueue����β����EnqueueHead����ͷ����
		*
		* @param newItem   Ҫ�������Ϣ
		* @param timeout   ��ʱʱ��
		*
		* @retval >0 �����е���Ϣ��Ŀ
		* @retval -1 ʧ�ܣ���ʱ�����ΪDEACTIVATE״̬��
		*/
		virtual int Enqueue(MESSAGE_TYPE& newItem, DWORD timeout = 0);
		virtual int EnqueueHead(MESSAGE_TYPE& newItem, DWORD timeout = 0);

		/**
		* �Ӷ�����ȡ��һ����Ϣ��Dequeue��ͷ��ȡ��DequeueHead��β��ȡ��
		*
		* @param dequeued  ����ȡ������Ϣ
		* @param timeout   ��ʱʱ��
		*
		* @retval >0 �����е���Ϣ��Ŀ
		* @retval -1 ʧ�ܣ���ʱ�����ΪDEACTIVATE״̬��
		*/
		virtual int Dequeue(MESSAGE_TYPE& dequeued, DWORD timeout = 0);
		virtual int DequeueTail(MESSAGE_TYPE& dequeued, DWORD timeout = 0);

		/*
		* ��������е���Ϣ
		* @retval The number of messages flushed.
		*/
		virtual int Flush(void);

		/// True if queue is full, else false.
		virtual bool IsFull(void);

		/// True if queue is empty, else false.
		virtual bool IsEmpty(void);

		/**
		* ���ظ�ˮλ��
		*/
		virtual size_t HighWaterMark(void);

		/**
		* ���ø�ˮλ�ֻ꣬Ҫ�����е���Ϣ��Ŀ���ڻ���ڸ�ֵ����ʱ����Ӳ�����
		* ����������ֱ����Ϣ��Ŀ���ڻ���ڵ�ˮλ��ʱ�ŷ��ء�
		*/
		virtual void HighWaterMark(size_t hwm);

		/**
		* ���ص�ˮλ��
		*/
		virtual size_t LowWaterMark(void);

		/**
		* ���õ�ˮλ�ֻ꣬�е������е���Ϣ��Ŀ���ڻ���ڸ�ֵʱ�������ڸö�����
		* ����Ӳ����Ż�õ�֪ͨ�����ء�
		*/
		virtual void LowWaterMark(size_t lwm);

		/// ���ض����е���Ϣ��Ŀ
		virtual size_t GetMessageCount(void);

		/**
		* ����������Ϊ�ǻ״̬�����һ������������ڶ����ϵ��̲߳����������е�
		* ��Ϣ���ᱻ�Ƴ���
		*
		* @retval  ����֮ǰ��״̬
		*/
		virtual int Deactivate(void);

		/**
		* ������У�ʹ�����ϵ���Ӻͳ��Ӳ������������С�
		*
		* @retval  ����֮ǰ��״̬
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

		/// ���ض��еĵ�ǰ״̬
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