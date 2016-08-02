#define NO_NEW_OVERDEFINE
#include "memory.h"
#undef NO_NEW_OVERDEFINE

//#if 0

//#define NO_ALLOC_MAPING

// http://stackoverflow.com/questions/438515/how-to-track-memory-allocations-in-c-especially-new-delete

#include <cstdio>
#include <cstdlib>
#include <cstring>

//#include <hash_map>
#include <map>

#include <windows.h>

using namespace std;

///@todo memory alloc tesztet singleton osztallyal kell megcsinalni -> konstrukt es destrukt automatikusan

#ifndef DEBUG_NO_MEMORY_ALLOC_TRACKING
#ifdef CORE_DUMPING

#include "memory_alloc.h"

//#define ALLOC_TRACKING
#define ALLOC_TRACKING_V2

namespace {
	void add(const char *msg, unsigned int size, const char *file, int line, void *ptr){
		MallocTrk::instance().add(msg, size, file, line, ptr);
	}

	void remove(const char *msg, const char *file, int line, void *ptr){
		MallocTrk::instance().remove(msg, file, line, ptr);
	}

	void resize(const char *msg, int newsize, const char *file, int line, void *ptr, void *newptr){
		MallocTrk::instance().resize(msg, newsize, file, line, ptr, newptr);
	}
}

void* __cdecl operator new(unsigned int size, const char *file, int line){
	void *p = malloc(size); //memset(p, 0, size);
#ifdef MEMORY_CLEAR_ALLOC
	memset(p, 0, size);
#endif 
	add("new", size, file, line, p);
	return p;
}

namespace{
	static char __deleteFile[4096];
	static int __deleteLine;
}

void __cdecl operator delete(void *p){
	void *pp = p;
	remove("delete", __deleteFile, __deleteLine, p);
	free(pp);
}

// ez kisse fura megoldas
void __cdecl deleteTracker(const char *file, int line){
#if defined ALLOC_TRACKING || defined ALLOC_TRACKING_V2
	strcpy(__deleteFile, file);
	__deleteLine = line;
#endif
}

/////////////////////////////////////////////////////////////////////////////////

void *DBG_malloc(size_t size, const char *file, int line){
	void *p = malloc(size); //memset(p, 0, size);
#ifdef MEMORY_CLEAR_ALLOC
	memset(p, 0, size);
#endif
	add("malloc", size, file, line, p);
	return p;
}

void *DBG_realloc (void *ptr, size_t size, const char *file, int line){
	void *p = ptr, *pp = NULL;
	size_t oldsize = 0, newsize = size;

	p = realloc(ptr, size);

	resize("realloc", size, file, line, ptr, p);
	return p;
}

void *DBG_calloc(size_t count, size_t size, const char *file, int line){
	void *p = calloc(count, size); //memset(p, 0, size);
	add("calloc", size, file, line, p);
	return p;
}

void  DBG_free(void *p, const char *file, int line){
	void *pp = p;
	remove("free", file, line, pp);
	free(pp);
}

#endif /*CORE_DUMPING*/
#endif /*DEBUG_NO_MEMORY_ALLOC_TRACKING*/