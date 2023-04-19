#ifndef _PRIORITYQUEUE_CPP_
#define _PRIORITYQUEUE_CPP_

#include ".\priorityqueue.h"

namespace dbe
{
	template<class value_type, class visitor_type> inline
	PriorityQueue<value_type, visitor_type>::PriorityQueue(void)
	{
	}

	template<class value_type, class visitor_type> inline
	PriorityQueue<value_type, visitor_type>::~PriorityQueue(void)
	{
	}

	template<class value_type, class visitor_type> inline
	bool PriorityQueue<value_type, visitor_type>::empty(void) const
	{
		return m_array.empty();
	}

	template<class value_type, class visitor_type> inline
	size_t PriorityQueue<value_type, visitor_type>::size(void) const
	{
		return m_array.size();
	}

	template<class value_type, class visitor_type> inline
	void PriorityQueue<value_type, visitor_type>::erase_all(void)
	{
		return m_array.erase(m_array.begin(), m_array.end());
	}

	template<class value_type, class visitor_type> inline
	void PriorityQueue<value_type, visitor_type>::clear(void)
	{
		return m_array.clear();
	}

	template<class value_type, class visitor_type>
	void PriorityQueue<value_type, visitor_type>::pop(void)
	{
		assert(!m_array.empty());
		value_type& last = m_array.back();
		size_t count = m_array.size();
		size_t i=1;
		for(  i=1; i*2 < count;)
		{
			size_t child = i*2;
			if(child + 1 < count && m_array[child] < m_array[child-1])
				++child;
			if(last < m_array[child-1])
				break;
			m_array[i-1] = m_array[child-1];
			i = child;
		}
		m_array[i-1] = last;
		m_array.pop_back();
	}

	template<class value_type, class visitor_type> inline
	const value_type& PriorityQueue<value_type, visitor_type>::top(void) const
	{
		assert(!m_array.empty());
		return m_array.front();
	}

	template<class value_type, class visitor_type>
	const value_type& PriorityQueue<value_type, visitor_type>::push(const value_type& _Pred)
	{
		m_array.push_back(_Pred);
		size_t i;
		for( i= m_array.size(); i > 1 && _Pred < m_array[i/2-1]; )
		{
			m_array[i-1] = m_array[i/2-1];
			i /= 2;
		}

		m_array[i-1] = _Pred;

		return m_array[i-1];
	}

	template<class value_type, class visitor_type>
	void PriorityQueue<value_type, visitor_type>::accept(visitor_type& visitor) const
	{
		for(size_t i=0; i<m_array.size(); ++i)
		{
			visitor(m_array[i]);
		}
	}
}

#endif