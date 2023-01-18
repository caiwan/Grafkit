#ifndef MEMORY_TRACKING_MODULE_H__
#define MEMORY_TRACKING_MODULE_H__

#include "../GKSDK_config.h"

#include <cstdlib>

///////////////////////////////////////////////////////////////////////
#ifndef DEBUG_NO_MEMORY_ALLOC_TRACKING
#ifdef CORE_DUMPING

//#ifndef __cplusplus
//void *__cdecl operator new[](unsigned int size); //fuck you dry
//void *__cdecl operator new[](unsigned int size, const char *file, int line);
void *__cdecl operator new(unsigned int size, const char *file, int line);
void __cdecl operator delete(void *p); // ??
//inline void __cdecl operator delete[](void *p){operator delete(p);}
extern void __cdecl deleteTracker(const char *file, int line);
//#endif /*__cplusplus*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

extern void * DBG_malloc  (size_t size,               const char *file, int line);
extern void * DBG_realloc (void *ptr, size_t size,    const char *file, int line);
extern void * DBG_calloc  (size_t count, size_t size, const char *file, int line);
extern void   DBG_free    (void *ptr,                 const char *file, int line);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*CORE_DUMPING*/

#else /*DEBUG_NO_MEMORY_ALLOC_TRACKING*/
#define NO_NEW_OVERDEFINE
#endif /*DEBUG_NO_MEMORY_ALLOC_TRACKING*/

extern void write_allocationTable();

#ifndef NO_NEW_OVERDEFINE

//#ifndef __cplusplus
#ifdef CORE_DUMPING
#define DEBUG_NEW new(__FILE__, __LINE__)
#define DEBUG_DELETE deleteTracker(__FILE__, __LINE__), delete
//#define DEBUG_DELETE_A deleteTracker(__FILE__, __LINE__), delete[]
//#endif /*__cplusplus*/

#define DBG_MALLOC(size)			DBG_malloc(size,		__FILE__, __LINE__)
#define DBG_CALLOC(count, size)		DBG_calloc(count, size, __FILE__, __LINE__)
#define DBG_REALLOC(ptr, newsize)	DBG_realloc(ptr, newsize, __FILE__, __LINE__)
#define DBG_FREE(ptr)				DBG_free(ptr, __FILE__, __LINE__)

#else /*CORE_DUMPING*/

#define DEBUG_NEW new
#define DEBUG_DELETE delete
//#define DEBUG_DELETE_A delete[]

#define DBG_MALLOC(size) malloc(size)
#define DBG_CALLOC(count, size) calloc(count, size)
#define DBG_REALLOC(ptr, newsize) realloc(ptr, newsize)
#define DBG_FREE(ptr) free(ptr)

#endif /*CORE_DUMPING*/

//#ifndef __cplusplus
#define new DEBUG_NEW
#define delete DEBUG_DELETE
//#define delete[] DEBUG_DELETE_A
//#endif /*__cplusplus*/

#define malloc DBG_MALLOC
#define calloc DBG_CALLOC
#define realloc DBG_REALLOC
#define free DBG_FREE

#endif /*NO_NEW_OVERDEFINE*/

#endif /*MEMORY_TRACKING_MODULE_H__*/
///////////////////////////////////////////////////////////////////////