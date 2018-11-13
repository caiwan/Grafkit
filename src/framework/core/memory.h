#pragma once 

#include <exception>

#define _SILENCE_STDEXT_ALLOCATORS_DEPRECATION_WARNING
#include <allocators>

#pragma warning(push)
#pragma warning(disable : 5040)

template<typename TDerived> struct AlignedNew
{
	// Allocate aligned memory.
    void* operator new (size_t size) throw(std::bad_alloc)
	{
		const size_t alignment = __alignof(TDerived);
		static_assert(alignment > 8, "AlignedNew is only useful for types with > 8 byte alignment. Did you forget a __declspec(align) on TDerived?");

		void* mem = _aligned_malloc(size, alignment);

		if (!mem)
			throw std::bad_alloc();

		return mem;
	}

	// Free aligned memory.
    void operator delete (void* ptr) throw()
	{
		_aligned_free(ptr);
	}


	// Array overloads.
    void* operator new[](size_t size) 
	{
		return operator new(size);
	}

    void operator delete[](void* ptr)
	{
		operator delete(ptr);
	}

};

#pragma warning(pop)