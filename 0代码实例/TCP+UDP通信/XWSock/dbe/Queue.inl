#ifndef _DBE_QUEUE_CPP_
#define _DBE_QUEUE_CPP_

#include <cassert>
#include "Queue.h"

namespace dbe
{
	template <class T, class VISITOR>
	QueueAsArray<T, VISITOR>::QueueAsArray(size_t size)
	: m_array(size == -1 ? 32 : size)
	, m_head(0)
	, m_tail(size == -1 ? 31 : size-1)
	, m_count(0)
	, m_size(size)
	{
	}

	template <class T, class VISITOR>
	QueueAsArray<T, VISITOR>::QueueAsArray(const QueueAsArray& other)
	: m_array(other.m_array)
	, m_head(other.m_head)
	, m_tail(other.m_tail)
	, m_count(other.m_count)
	, m_size(other.m_size)
	{
	}

	template <class T, class VISITOR>
	QueueAsArray<T, VISITOR>& QueueAsArray<T, VISITOR>::operator = (const QueueAsArray& other)
	{
		if(this == &other) return *this;
		m_array = other.m_array;
		m_head = other.m_head;
		m_tail = other.m_tail;
		m_count = other.m_count;
		m_size = other.m_size;
	}

	template <class T, class VISITOR> inline
	QueueAsArray<T, VISITOR>::~QueueAsArray()
	{	Clear();	}

	template <class T, class VISITOR> inline
	T& QueueAsArray<T, VISITOR>::GetHead()
	{
		assert(m_count > 0);
		return m_array[m_head];
	}

	template <class T, class VISITOR> inline
	T& QueueAsArray<T, VISITOR>::GetTail()
	{
		assert(m_count > 0);
		return m_array[m_tail];
	}

	template <class T, class VISITOR>
	bool QueueAsArray<T, VISITOR>::Enqueue(T& item)
	{
		if(m_count >= m_array.GetLength() && m_size == -1)
		{
			Resize_i(m_count + 64);
		}
		if(m_count < m_array.GetLength())
		{
			if(++m_tail == m_array.GetLength()) m_tail = 0;
			m_array[m_tail] = item;
			++m_count;
			return true;
		}
		return false;
	}

	template <class T, class VISITOR>
	bool QueueAsArray<T, VISITOR>::EnqueueHead(T& item)
	{
		if(m_count >= m_array.GetLength() && m_size == -1)
		{
			Resize_i(m_count + 64);
		}
		if(m_count < m_array.GetLength())
		{
			if(m_head-- == 0) m_head = m_array.GetLength()-1;
			m_array[m_head] = item;
			++m_count;
			return true;
		}
		return false;
	}

	template <class T, class VISITOR>
	bool QueueAsArray<T, VISITOR>::Dequeue()
	{
		if(m_count > 0)
		{
			m_array[m_head] = T();
			if(++m_head == m_array.GetLength()) m_head = 0;
			--m_count;
			return true;
		}
		return false;
	}

	template <class T, class VISITOR>
	bool QueueAsArray<T, VISITOR>::DequeueTail()
	{
		if(m_count > 0)
		{
			m_array[m_tail] = T();
			if(m_tail-- == 0) m_tail = m_array.GetLength() - 1;
			--m_count;
			return true;
		}
		return false;
	}

	template <class T, class VISITOR>
	void QueueAsArray<T, VISITOR>::Resize(size_t newSize)
	{
		if(newSize > m_array.GetLength())
		{
			m_size = newSize;
			if(m_size == -1)
			{
				if(m_count >= m_array.GetLength())
				{
					Resize_i(m_count + 64);
				}
			}
			else
			{
				Resize_i(newSize);
			}
		}
	}

	template <class T, class VISITOR>
	void QueueAsArray<T, VISITOR>::Resize_i(size_t newSize)
	{
		size_t oldLen = m_array.GetLength();
		if(newSize > oldLen)
		{
			QueueAsArray<T, VISITOR> tmpQueue(*this);
			m_array.SetLength(newSize);
			m_head = m_count = 0;
			m_tail = newSize - 1;

			for(size_t i=0; i<oldLen; i++)
			{
				if(++m_tail == m_array.GetLength()) m_tail = 0;
				m_array[m_tail] = tmpQueue.GetHead();
				++m_count;

				tmpQueue.Dequeue();
			}
		}
	}

	template <class T, class VISITOR> inline
	size_t QueueAsArray<T, VISITOR>::GetCount() const
	{
		return m_count;
	}

	template <class T, class VISITOR> inline
	size_t QueueAsArray<T, VISITOR>::GetSize() const
	{
		return m_array.GetLength();
	}

	template <class T, class VISITOR> inline
	bool QueueAsArray<T, VISITOR>::IsEmpty() const
	{
		return (m_count == 0);
	}

	template <class T, class VISITOR> inline
	bool QueueAsArray<T, VISITOR>::IsFull() const
	{
		return (m_count >= m_array.GetLength());
	}

	template <class T, class VISITOR>
	void QueueAsArray<T, VISITOR>::Clear()
	{
		T item = T();
		m_head = 0;
		m_tail = m_array.GetLength() - 1;
		m_count = 0;
		for(size_t i=0; i<=m_tail; i++)
		{
			m_array[i] = item;
		}
	}

	template <class T, class VISITOR>
	void QueueAsArray<T, VISITOR>::Accept(VISITOR& visitor)
	{
		if(m_count > 0)
		{
			if(m_head <= m_tail)
			{
				for(size_t i=m_head; i<=m_tail; i++)
				{
					visitor(m_array[i]);
				}
			}
			else
			{
				for(size_t i=m_head; i<m_array.GetLength(); i++)
				{
					visitor(m_array[i]);
				}
				for(size_t i=0; i<=m_tail; i++)
				{
					visitor(m_array[i]);
				}
			}
		}
	}
}

#endif