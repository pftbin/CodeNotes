#ifndef _DBE_MESSAGEQUEUE_CPP_
#define _DBE_MESSAGEQUEUE_CPP_

#include "MessageQueue.h"

namespace dbe
{
	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	MessageQueue(size_t hwm, size_t lwm)
	: m_state(ACTIVATED)
	, m_queue(hwm)
	, m_highWaterMark(hwm)
	, m_lowWaterMark(lwm)
	, m_flushCallBack(NULL)
	, m_pFuncParam(NULL)
	{
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	SetFlushCallBack(FLUSHCALLBACK pfn, void* pParam)
	{
		Guard guard(m_lock);
		m_flushCallBack = pfn;
		m_pFuncParam = pParam;
		return 0;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Close(void)
	{
		Guard guard(m_lock);

		int result = Deactivate_i();

		// Free up the remaining messages on the queue.
		Flush_i ();

		return result;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	~MessageQueue(void)
	{
		Close();
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	PeekDequeueHead(MESSAGE_TYPE& firstItem, DWORD timeout)
	{
		Guard guard(m_lock);

		if(m_state == DEACTIVATED)
			return -1;

		if(WaitNotEmpty(timeout) == -1)
			return -1;

		int count = (int)m_queue.GetCount();
		if(count > 0)
		{
			firstItem = m_queue.GetHead();
		}

		return count;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Enqueue(MESSAGE_TYPE& newItem, DWORD timeout)
	{
		int count = 0;
		{
			Guard guard(m_lock);

			if(m_state == DEACTIVATED)
				return -1;

			if(WaitNotFull(timeout) == -1)
				return -1;

			count = Enqueue_i(newItem);
		}
		return count;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	EnqueueHead(MESSAGE_TYPE& newItem, DWORD timeout)
	{
		int count = 0;
		{
			Guard guard(m_lock);

			if(m_state == DEACTIVATED)
				return -1;

			if(WaitNotFull(timeout) == -1)
				return -1;

			count = EnqueueHead_i(newItem);
		}

		return count;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	DequeueTail(MESSAGE_TYPE& dequeued, DWORD timeout)
	{
		Guard guard(m_lock);

		if(m_state == DEACTIVATED)
			return -1;

		if(WaitNotEmpty(timeout) == -1)
			return -1;

		return DequeueTail_i(dequeued);
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Dequeue(MESSAGE_TYPE& dequeued, DWORD timeout)
	{
		Guard guard(m_lock);

		if (m_state == DEACTIVATED)
			return -1;

		if (WaitNotEmpty (timeout) == -1)
			return -1;

		return Dequeue_i(dequeued);
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Flush (void)
	{
		Guard guard(m_lock);
		return Flush_i();
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	bool MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	IsFull(void)
	{
		Guard guard(m_lock);
		return IsFull_i();
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	bool MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	IsEmpty(void)
	{
		Guard guard(m_lock);
		return IsEmpty_i();
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	size_t MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	HighWaterMark(void)
	{
		Guard guard(m_lock);
		return m_highWaterMark;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	void MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	HighWaterMark(size_t hwm)
	{
		Guard guard(m_lock);
		m_highWaterMark = hwm;
		if(hwm > m_queue.GetSize())
		{
			m_queue.Resize(hwm);
		}
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	size_t MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	LowWaterMark(void)
	{
		Guard guard(m_lock);
		return m_lowWaterMark;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	void MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	LowWaterMark(size_t lwm)
	{
		Guard guard(m_lock);
		m_lowWaterMark = lwm;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	size_t MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	GetMessageCount(void)
	{
		Guard guard(m_lock);
		return (int)m_queue.GetCount();
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Deactivate(void)
	{
		Guard guard(m_lock);
		return Deactivate_i(0);
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Activate(void)
	{
		Guard guard(m_lock);
		return Activate_i();
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Pulse(void)
	{
		Guard guard(m_lock);
		return Deactivate_i(1);
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	GetState(void)
	{
		Guard guard(m_lock);
		return m_state;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	bool MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	IsDeactivated(void)
	{
		Guard guard(m_lock);
		return (m_state == DEACTIVATED);
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Flush_i (void)
	{
		int numberFlushed = (int)m_queue.GetCount();

		while(!m_queue.IsEmpty())
		{
			DESTRUCT_STRATEGY::Release(m_queue.GetHead());
			m_queue.Dequeue();
		}

		return numberFlushed;
		/*int numberFlushed = (int)m_queue.GetCount();
		MESSAGE_TYPE* pMsgs = new MESSAGE_TYPE[numberFlushed];

		for(int i=0; !m_queue.IsEmpty(); ++i)
		{
			pMsgs[i] = m_queue.GetHead();
			m_queue.Dequeue();
		}

		if(m_flushCallBack != NULL && numberFlushed > 0)
		{
			m_flushCallBack(m_pFuncParam, pMsgs, numberFlushed);
		}

		for(i--; i>=0; i--)
		{
			DESTRUCT_STRATEGY::Release(pMsgs[i]);
		}

		delete [] pMsgs;

		return numberFlushed;*/
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Enqueue_i (MESSAGE_TYPE& newItem)
	{
		if (!m_queue.Enqueue(newItem) || SignalDequeueWaiters() == -1)
			return -1;

		return (int)m_queue.GetCount();
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	EnqueueHead_i (MESSAGE_TYPE& newItem)
	{
		if (!m_queue.EnqueueHead(newItem) || SignalDequeueWaiters() == -1)
			return -1;

		return (int)m_queue.GetCount();
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Dequeue_i (MESSAGE_TYPE& dequeued)
	{
		if(m_queue.IsEmpty()) return -1;
		dequeued = m_queue.GetHead();
		m_queue.Dequeue();
		size_t count = m_queue.GetCount();
		if(count <= m_lowWaterMark && SignalEnqueueWaiters() == -1)
			return -1;
		return (int)count;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	DequeueTail_i (MESSAGE_TYPE& dequeued)
	{
		if(m_queue.IsEmpty()) return -1;
		dequeued = m_queue.GetTail();
		m_queue.DequeueTail();
		size_t count = m_queue.GetCount();
		if(count <= m_lowWaterMark && SignalEnqueueWaiters() == -1)
			return -1;
		return (int)count;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Deactivate_i (int pulse)
	{
		int prevState = m_state;

		if (prevState != DEACTIVATED)
		{
			if (pulse)
				m_state = PULSED;
			else
				m_state = DEACTIVATED;

			// Wakeup all waiters.
			m_notEmptyCond.Broadcast();
			m_notFullCond.Broadcast();
		}
		return prevState;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	bool MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	IsFull_i (void)
	{
		return (m_queue.GetCount() >= m_highWaterMark);
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	bool MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	IsEmpty_i (void)
	{
		return (m_queue.GetCount() == 0);
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	Activate_i (void)
	{
		int prevState = m_state;
		m_state = ACTIVATED;
		return prevState;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	WaitNotFull (DWORD timeout)
	{
		int result = 0;

		while(IsFull_i())
		{
			if(!m_notFullCond.Wait(m_lock, timeout))
			{
				result = -1;
				break;
			}
			if(m_state == DEACTIVATED)
			{
				result = -1;
				break;
			}
			else if(m_state == PULSED)
			{
				result = 0;
				break;
			}
		}
		return result;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	WaitNotEmpty (DWORD timeout)
	{
		int result = 0;

		while(IsEmpty_i())
		{
			if(!m_notEmptyCond.Wait(m_lock, timeout))
			{
				result = -1;
				break;
			}
			if(m_state == DEACTIVATED)
			{
				result = -1;
				break;
			}
			else if(m_state == PULSED)
			{
				result = 0;
				break;
			}
		}
		return result;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
	int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
	SignalEnqueueWaiters (void)
	{
		if(!m_notFullCond.Signal())
			return -1;
		return 0;
	}

	template <class MESSAGE_TYPE, class DESTRUCT_STRATEGY, class SYNCH_STRATEGY>
		int MessageQueue<MESSAGE_TYPE, DESTRUCT_STRATEGY, SYNCH_STRATEGY>::
		SignalDequeueWaiters (void)
	{
		if(!m_notEmptyCond.Signal())
			return -1;
		return 0;
	}
}

#endif