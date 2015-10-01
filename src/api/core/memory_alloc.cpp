#include "memory_alloc.h"

MallocTrk::MallocTrk() :
	allocatedmem(0), peakmem(0), sumalloc(0), opcount(),
	fp(NULL)
{
	fp = fopen(MALLOC_TRK_FILENAME, "wb");
	if (!fp) throw 1;	// ha hiba tortenik, abbol unhandled exception lesz-> remelhetoleg meg lehet majd talalni
}

MallocTrk::~MallocTrk(){
	fflush(this->fp);
	fclose(this->fp);

	write_allocationTable();
} ///@todo < ide egy badptr pottyan

void MallocTrk::memorylog(const char *msg, unsigned int size, const char *file, int line, void *ptr){
		char buffer[4096];
		
		if (peakmem < allocatedmem) peakmem = allocatedmem;

		if (this->mutex) while(!this->mutex);
		this->mutex++;

		{
			sprintf(buffer, "%s: %d bytes @0X%08x at file %s line %d. Sum: %d bytes\n\0", msg, size, ptr, file, line, allocatedmem);
			fputs(buffer, fp);
			fflush(fp);
		}
		this->mutex--;
}

void MallocTrk::add(const char *msg, unsigned int size, const char *file, int line, void *ptr){
//#ifndef NO_ALLOC_MAPING
		memory_token_t tmp;
		
		strcpy(tmp.filename, file);
		strcpy(tmp.comment, msg);
		tmp.line = line;
		tmp.size = size;

		sumalloc += size;
		allocatedmem += size;

		opcount ++;

		memoryMap[ptr] = tmp;
//#endif /*NO_ALLOC_MAPING*/

		memorylog(msg, size, file, line, ptr);
}

void MallocTrk::remove(const char *msg, const char *file, int line, void *ptr){
		// kivesz
//#ifndef NO_ALLOC_MAPING	
		if (memoryMap.empty()) 
			return;

		int keres = memoryMap.count(ptr);
		if (memoryMap.count(ptr)==0) 
			return;
		
		memory_map_t::iterator it = memoryMap.find(ptr);
		memory_token_t tmp = it->second;

		allocatedmem -= tmp.size;

		opcount ++;

		memoryMap.erase(it);
		memorylog(msg, tmp.size, file, line, ptr);
//#else /*NO_ALLOC_MAPING*/
//		memorylog(msg, -1, file, line, ptr);
//#endif /*NO_ALLOC_MAPING*/
}

void MallocTrk::resize(const char *msg, int newsize, const char *file, int line, void *ptr, void *newptr){
			// kivesz
///#ifndef NO_ALLOC_MAPING
		if (memoryMap.empty()) 
			return;

		int keres = memoryMap.count(ptr);
		if (memoryMap.count(ptr) == 0) {
			add(msg, newsize, file, line, newptr);
			return;
		}

		memory_map_t::iterator it = memoryMap.find(ptr);
		memory_token_t &tmp = it->second;

		sumalloc += newsize - tmp.size;
		allocatedmem += newsize - tmp.size;

		opcount ++;

		// csere
		strcpy(tmp.filename, file);
		strcpy(tmp.comment, msg);
		tmp.line = line;
		tmp.size = newsize;
		
		if (ptr != newptr)
		{
			memory_token_t ttmp;
			memcpy(&ttmp, &tmp, sizeof(tmp));

			memoryMap.erase(it);

			memoryMap[newptr] = ttmp;
		}

///#endif /*NO_ALLOC_MAPING*/
		memorylog(msg, newsize, file, line, newptr);
}

///@todo alloc maping nelkul is menjen
///#ifndef NO_ALLOC_MAPING
///#ifdef CORE_DUMPING

///#endif
///#endif /*NO_ALLOC_MAPING*/


void MallocTrk::write_allocationTable(){
	FILE* fp_ = fopen(MALLOC_TRK_LEAK_FILENAME, "wb");
	if (!fp_) throw 1;

	int mem = 0;

	fprintf(fp_, "Peak memory usage: %d bytes \n", peakmem);
	fprintf(fp_, "Allocation: %d bytes \n", sumalloc);
	fprintf(fp_, "Number of memory operations: %d \n", opcount);
	fprintf(fp_,  "\n---------------------------------------------------------\n");

	if (memoryMap.empty()) {
		fprintf(fp_, "No memory leak detected, you are awesome, yey :3\n");

	} else {
		for (memory_map_t::iterator it = memoryMap.begin(); it!=memoryMap.end(); ++it){
			void * ptr = it->first;
			memory_token_t &tmp = it->second;

			mem += tmp.size;

			fprintf(fp_, "%s: %d bytes @0X%08x at file %s line %d. Sum: %d bytes\n", 
				tmp.comment, tmp.size, ptr, tmp.filename, tmp.line, mem);
		}
	}

	fflush(fp_);
	fclose(fp_);
}