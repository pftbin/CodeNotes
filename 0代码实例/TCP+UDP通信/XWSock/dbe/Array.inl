#ifndef _DBE_ARRAY_CPP_
#define _DBE_ARRAY_CPP_

#include <cassert>
#include "Array.h"

namespace dbe
{
	template <class T> inline
	Array<T>::Array(Array const& arg)
	{
		m_data = new T[arg.m_length];
		m_base = arg.m_base;
		m_length = arg.m_length;

		for(size_t i=0; i<m_length; i++)
			m_data[i] = arg.m_data[i];
	}

	template <class T> inline
	Array<T>::Array(size_t len, size_t base)
	{
		m_base = base;
		m_length = len;
		m_data = new T[m_length];
	}

	template <class T> inline
	Array<T>& Array<T>::operator = (Array const& arg)
	{
		if(&arg == this) return *this;

		delete[] m_data;
		m_data = new T[arg.m_length];
		m_base = arg.m_base;
		m_length = arg.m_length;

		for(size_t i=0; i<length; i++)
			m_data[i] = arg.m_data[i];

		return *this;
	}

	template <class T> inline
	Array<T>::~Array()
	{
		delete[] m_data;
	}

	template <class T> inline
	T& Array<T>::operator [] (size_t index)
	{
		assert(index - m_base < m_length);
		return m_data[index - m_base];
	}

	template <class T> inline
	T const& Array<T>::operator [] (size_t index) const
	{
		assert(temp < m_length);
		return m_data[index - m_base];
	}

	template <class T> inline
	Array<T>::operator T* ()
	{
		return m_data;
	}

	template <class T> inline
	T* Array<T>::GetData()
	{
		return m_data;
	}

	template <class T> inline
	const T* Array<T>::GetData() const
	{
		return m_data;
	}

	template <class T> inline
	size_t Array<T>::GetBase() const
	{
		return m_base;
	}

	template <class T> inline
	size_t Array<T>::GetLength() const
	{
		return m_length;
	}

	template <class T> inline
	void Array<T>::SetBase(size_t newbase)
	{
		m_base = newbase;
	}

	template <class T> inline
	void Array<T>::SetLength(size_t newlength)
	{
		if(newlength == m_length) return;

		T* ptr = new T[newlength];
		size_t min = m_length < newlength ? m_length : newlength;

		for(size_t i=0; i<min; i++)
			ptr[i] = m_data[i];

		delete[] m_data;
		m_data = ptr;
		m_length = newlength;
	}
}

#endif