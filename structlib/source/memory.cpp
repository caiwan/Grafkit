/// http://stackoverflow.com/questions/438515/how-to-track-memory-allocations-in-c-especially-new-delete

#define NO_NEW_OVERDEFINE
#include "memory.h"
#undef NO_NEW_OVERDEFINE


#ifdef NO_MEMORY_TRACKING
#define TRACK(x) x

#include "memory_alloc.h"

namespace {
	void add(const char *msg, size_t size, const char *file, int line, void *ptr) {
		MallocTrk::instance().add(msg, size, file, line, ptr);
	}

	void remove(const char *msg, const char *file, int line, void *ptr) {
		MallocTrk::instance().remove(msg, file, line, ptr);
	}

	void resize(const char *msg, int newsize, const char *file, int line, void *ptr, void *newptr) {
		MallocTrk::instance().resize(msg, newsize, file, line, ptr, newptr);
	}

	namespace {
		///@todo nem threadsafe
		static char __deleteFile[4096];
		static int __deleteLine;
	}

	void __cdecl deleteTracker(const char *file, int line) {
		///@todo nem threadsafe
		strcpy(__deleteFile, file);
		__deleteLine = line;
	}
}

#else // NO_MEMORY_TRACKING
#define TRACK(x)
#endif // NO_MEMORY_TRACKING


#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <windows.h>

using namespace std;

/* *************************************************************************************
 New, delete
*/

void* __cdecl operator new(size_t size, const char *file, int line){
	void *p = malloc(size); //memset(p, 0, size);

/// @todo add aligned new 

	TRACK(add("new", size, file, line, p));
	return p;
}

void __cdecl operator delete(void *p){
	void *pp = p;
	TRACK(remove("delete", __deleteFile, __deleteLine, p));
	free(pp);
}

/* *************************************************************************************
Malloc, realloc, calloc, free
*/

void  *_DBG_malloc(size_t size, const char *file, int line){
	void *p = malloc(size); 

	TRACK(add("malloc", size, file, line, p));
	return p;
}

void *_DBG_realloc (void *ptr, size_t size, const char *file, int line){
	void *p = ptr, *pp = NULL;
	size_t oldsize = 0, newsize = size;

	p = realloc(ptr, size);

	TRACK(resize("realloc", size, file, line, ptr, p));
	return p;
}

void *_DBG_calloc(size_t count, size_t size, const char *file, int line){
	void *p = calloc(count, size); 
	TRACK(add("calloc", size, file, line, p));
	return p;
}

void _DBG_free(void *p, const char *file, int line){
	void *pp = p;
	TRACK(remove("free", file, line, pp));
	free(pp);
}
