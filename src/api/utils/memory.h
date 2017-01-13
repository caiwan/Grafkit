#pragma once 

#include <malloc.h>
#include <exception>
#include <map>
#include <allocators>

#include "logger.h"


template<typename TDerived> struct AlignedNew
{
	// Allocate aligned memory.
	static void* operator new (size_t size) throw(std::bad_alloc)
	{
		const size_t alignment = __alignof(TDerived);
		static_assert(alignment > 8, "AlignedNew is only useful for types with > 8 byte alignment. Did you forget a __declspec(align) on TDerived?");

		void* mem = _aligned_malloc(size, alignment);

		if (!mem)
			throw std::bad_alloc();

		return mem;
	}

	// Free aligned memory.
	static void operator delete (void* ptr) throw()
	{
		_aligned_free(ptr);
	}


	// Array overloads.
	static void* operator new[](size_t size) 
	{
		return operator new(size);
	}

	static void operator delete[](void* ptr)
	{
		operator delete(ptr);
	}

};

