/* -*- C++ -*- */

//=============================================================================
/**
*  @file    Array.h
*
*  Array.h,v 1.00 2006/01/18
*
*  @author п╩нд <xiewen@163.com>
*/
//=============================================================================

#ifndef _DBE_ARRAY_HEAD_
#define _DBE_ARRAY_HEAD_
#pragma once

namespace dbe
{
	template <class T>
	class Array
	{
	public:
		Array(Array const& arg);
		Array(size_t size, size_t base = 0);
		Array& operator = (Array const& arg);
		~Array();

		T& operator [] (size_t index);
		T const& operator [] (size_t index) const;
		operator T*();

		T* GetData();
		const T* GetData() const;
		size_t GetBase() const;
		size_t GetLength() const;

		void SetBase(size_t newBase);
		void SetLength(size_t newLength);

	private:
		T* m_data;
		size_t m_base;
		size_t m_length;
	};
}

#include "Array.inl"

#endif