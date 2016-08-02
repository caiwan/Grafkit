#pragma once 

// maloloc tracker thing
///@todo ne keruljon forditasra ez a modul, ha nem trackel memoriafoglalast

#include <cstdio>
#include <map>

#define MALLOC_TRK_WRITE_BUFFER_MAX 1024
//#define MALLOC_TRK_WRITE_BUFFER_MAX 65536
#define MALLOC_TRK_FILENAME "memory.log"
#define MALLOC_TRK_LEAK_FILENAME "memoryleak.log"

class MallocTrk{
	private:
		typedef struct {
			char filename[256], comment[256];
			int line, size;
		} memory_token_t;
		
		typedef std::map<void*, memory_token_t> memory_map_t;

	public:
		void add(const char *msg, unsigned int size, const char *file, int line, void *ptr);
		void remove(const char *msg, const char *file, int line, void *ptr);
		void resize(const char *msg, int newsize, const char *file, int line, void *ptr, void *newptr);

		// singleton
		static MallocTrk& instance(){
			static MallocTrk mtrk;
			return mtrk;
		}

	private:
		MallocTrk();
		
		MallocTrk(const MallocTrk&);	// prevent copy and operator=
		MallocTrk&	operator=(const MallocTrk&);

		virtual ~MallocTrk();

		void memorylog(const char *msg, unsigned int size, const char *file, int line, void *ptr);
		void write_allocationTable();

	private:
		char writeBuffer[MALLOC_TRK_WRITE_BUFFER_MAX];
		unsigned short bufferPointer;
		int mutex;
		unsigned long int allocatedmem, peakmem, sumalloc, opcount;

		FILE* fp;

		memory_map_t memoryMap;
};