/* -*- C++ -*- */

//=============================================================================
/**
*  @file    Queue.h
*
*  Queue.h,v 1.00 2006/01/05
*
*  @author п╩нд <xiewen@163.com>
*/
//=============================================================================

#ifndef _DBE_QUEUE_HEAD_
#define _DBE_QUEUE_HEAD_
#pragma once
#include "Array.h"

namespace dbe
{
	template <class T, class VISITOR = void(*)(T&)>
	class QueueAsArray
	{
	public:
		QueueAsArray(size_t size = 32);
		QueueAsArray(const QueueAsArray& other);
		QueueAsArray& operator = (const QueueAsArray& other);
		~QueueAsArray();

		T& GetHead();
		T& GetTail();
		bool Enqueue(T& item);
		bool EnqueueHead(T& item);
		bool Dequeue();
		bool DequeueTail();
		void Resize(size_t newSize);
		size_t GetCount() const;
		size_t GetSize() const;
		bool IsEmpty() const;
		bool IsFull() const;
		void Clear();
		void Accept(VISITOR& visitor);

	protected:
		void Resize_i(size_t newSize);

	protected:
		Array<T> m_array;
		size_t m_head;
		size_t m_tail;
		size_t m_count;
		size_t m_size;
	};
}

#include "Queue.inl"

#endif