/* -*- C++ -*- */

//=============================================================================
/**
*  @file    PriorityQueue.h
*
*  PriorityQueue.h,v 1.00 2006/01/05
*
*  @author п╩нд <xiewen@163.com>
*/
//=============================================================================

#ifndef _PRIORITYQUEUE_HEAD_
#define _PRIORITYQUEUE_HEAD_
#pragma once
#include <functional>
#include <vector>

namespace dbe
{
	template<class value_type, class visitor_type = void(*)(const value_type&)>
	class PriorityQueue
	{	// priority queue implemented with a _Container
	public:
		PriorityQueue(void);
		~PriorityQueue(void);
		bool empty(void) const;
		size_t size(void) const;
		void erase_all(void);
		void clear(void);
		void pop(void);
		const value_type& top(void) const;
		const value_type& push(const value_type& _Pred);
		void accept(visitor_type& visitor) const;
	private:
		std::vector<value_type> m_array;
	};
}

#include "PriorityQueue.inl"

#endif
