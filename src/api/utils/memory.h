/**
	Quick and dirty hack to keep memory allocation tracked

	- macro `NO_NEW_OVERDEFINE` keeps operator new and delete clean
	- macro `NO_MEMORY_TRACKING` will opt out collecting tracking information
	- macro `NO_ALIGNED_ALLOC` will opt out allocation with 16 byte alignemnt
*/

#define NO_MEMORY_TRACKING

#ifndef __MEMORY_TRACKING_MODULE_H__
#define __MEMORY_TRACKING_MODULE_H__

#include <cstdlib>

///@todo wipe this, use crtdbg

///////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
void *__cdecl operator new(size_t size, const char *file, int line);
void __cdecl operator delete(void *p); // ??
extern void __cdecl deleteTracker(const char *file, int line);
#endif /*__cplusplus*/

///////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

extern void * _DBG_malloc  (size_t size,               const char *file, int line);
extern void * _DBG_realloc (void *ptr, size_t size,    const char *file, int line);
extern void * _DBG_calloc  (size_t count, size_t size, const char *file, int line);
extern void   _DBG_free    (void *ptr,                 const char *file, int line);

extern void * _DBG_aligned_malloc(size_t size, size_t alignment, const char *file, int line);
extern void   _DBG_aligned_free(void *ptr,                       const char *file, int line);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

///////////////////////////////////////////////////////////////////////


#ifndef NO_NEW_OVERDEFINE

#ifndef __cplusplus

#define DEBUG_NEW new(__FILE__, __LINE__)
#define DEBUG_DELETE deleteTracker(__FILE__, __LINE__), delete
#define new DEBUG_NEW
#define delete DEBUG_DELETE

#endif /*__cplusplus*/

#ifndef NO_MEMORY_TRACKING

#define DBG_MALLOC(size)			_DBG_malloc(size,		__FILE__, __LINE__)
#define DBG_CALLOC(count, size)		_DBG_calloc(count, size, __FILE__, __LINE__)
#define DBG_REALLOC(ptr, newsize)	_DBG_realloc(ptr, newsize, __FILE__, __LINE__)
#define DBG_FREE(ptr)				_DBG_free(ptr, __FILE__, __LINE__)

#define DBG_aligned_malloc(size, alignment)  _DBG_aligned_malloc(size, alignment, __FILE__, __LINE__)
#define DBG_aligned_free(ptr) 	 _DBG_aligned_free(ptr, __FILE__, __LINE__)

#define malloc DBG_MALLOC
#define calloc DBG_CALLOC
#define realloc DBG_REALLOC
#define free DBG_FREE

#define _aligned_malloc DBG_aligned_malloc
#define _aligned_free DBG_aligned_free

#else /*NO_MEMORY_TRACKING*/

// ...

#endif /*NO_MEMORY_TRACKING*/


#else /*NO_NEW_OVERDEFINE*/

// ... 

#endif /*NO_NEW_OVERDEFINE*/

#endif /*__MEMORY_TRACKING_MODULE_H__*/
