#pragma once 

#include <malloc.h>
#include <exception>
#include <map>
#include <allocators>

#include "logger.h"

/*
	http://stackoverflow.com/questions/438515/how-to-track-memory-allocations-in-c-especially-new-delete
	http://www.flipcode.com/archives/How_To_Find_Memory_Leaks.shtml
*/

#ifndef LIVE_RELEASE
template<typename T> struct track_alloc : std::allocator<T>
{
	typedef typename std::allocator<T>::pointer pointer;
	typedef typename std::allocator<T>::size_type size_type;

	template<typename U>
	struct rebind {
		typedef track_alloc<U> other;
	};

	track_alloc() {}

	template<typename U> track_alloc(track_alloc<U> const& u) :std::allocator<T>(u)
	{
	}

	pointer allocate(size_type size, std::allocator<void>::const_pointer = 0)
	{
		void * p = std::malloc(size * sizeof(T));
		if (p == 0) {
			throw std::bad_alloc();
		}
		return static_cast<pointer>(p);
	}

	void deallocate(pointer p, size_type) {
		std::free(p);
	}
};

struct track_record {
	track_record() :
		size(0),
		file(""),
		function(""),
		line(0)
	{
	}

	track_record(std::size_t size, std::string file, std::string function, unsigned int line) :
		size(size),
		file(file),
		function(function),
		line(line)
	{
	}

	std::size_t size;
	std::string file, function;
	unsigned int line;
};

typedef std::map< void*, track_record, std::less<void*>, track_alloc< std::pair<void* const, track_record> > > track_type;

struct track_printer {
	track_type * track;
	track_printer(track_type * track) : track(track) {}
	~track_printer();
};

extern track_type * get_map();

extern void add_alloc(void* mem, std::size_t size, const char* file, const char* function, unsigned int line);
extern void remove_alloc(void* mem);

extern void * __cdecl operator new(std::size_t size, const char* file, const char* function, unsigned int line) throw(std::bad_alloc);
extern void __cdecl operator delete(void * mem) throw();

#endif //LIVE_RELEASE

/**
===================================================================================================

Aligned new, taken from DXTK
https://raw.githubusercontent.com/Microsoft/DirectXTK/master/Src/AlignedNew.h

===================================================================================================
*/

/**
// Derive from this to customize operator new and delete for
// types that have special heap alignment requirements.
//
// Example usage:
//
//      __declspec(align(16)) struct MyAlignedType : public AlignedNew<MyAlignedType>
*/

template<typename TDerived> struct AlignedNew
{
	// Allocate aligned memory.
#ifndef LIVE_RELEASE
	static void* operator new (size_t size, const char* file, const char* function, unsigned int line) throw(std::bad_alloc)
#else // LIVE_RELEASE
	static void* operator new (size_t size) throw(std::bad_alloc)
#endif //LIVE_RELEASE
	{
		const size_t alignment = __alignof(TDerived);
		static_assert(alignment > 8, "AlignedNew is only useful for types with > 8 byte alignment. Did you forget a __declspec(align) on TDerived?");

		void* mem = _aligned_malloc(size, alignment);

		if (!mem)
			throw std::bad_alloc();

#ifndef LIVE_RELEASE
		add_alloc(mem, size, file, function, line);
#endif

		return mem;
	}

	// Free aligned memory.
	static void operator delete (void* ptr) throw()
	{
#ifndef LIVE_RELEASE
		remove_alloc(ptr);
#endif //LIVE_RELEASE

		_aligned_free(ptr);
	}


	// Array overloads.
#ifndef LIVE_RELEASE
	static void* operator new[](size_t size, const char* file, const char* function, unsigned int line) 
	{
		return operator new(size, file, function, line);
	}
#else //LIVE_RELEASE
	static void* operator new[](size_t size) 
	{
		return operator new(size);
	}
#endif // LIVE_RELEASE

	static void operator delete[](void* ptr)
	{
		operator delete(ptr);
	}

};

#ifndef LIVE_RELEASE
/* override new */
#ifndef NO_NEW_OVERRIDE

#ifdef __FUNCTION__
#define DEBUG_FUNCTION__ __FUNCTION__
#else //__FUNCTION__
#define DEBUG_FUNCTION__ ""
#endif //__FUNCTION__

#define DEBUG_NEW new(__FILE__, __FUNCTION__, __LINE__)

#define new DEBUG_NEW
#endif /*NO_NEW_OVERRIDE*/
#endif /*LIVE_RELEASE*/
