#include "../stdafx.h"

#include "memory_alloc.h"

#define __STDC_WANT_SECURE_LIB__
#include <stdio.h>

MallocTrk::MallocTrk() :
	allocatedmem(0), peakmem(0), sumalloc(0), opcount(),
	fp(NULL)
{
}

MallocTrk::~MallocTrk() {
	if (fp) {
		fflush(this->fp);
		fclose(this->fp);
		fp = NULL;
	}
	write_allocationTable();
} ///@todo < ide egy badptr pottyan

void MallocTrk::memorylog(const char *msg, unsigned int size, const char *file, int line, void *ptr) {
	char buffer[4096];

	// create file on first use 
	if (!fp) 
		if (fopen_s(&fp, MALLOC_TRK_FILENAME, "wb")) 
			throw 2;
	if (!fp) throw 1; // hope we'll find as unhandled exception

	if (peakmem < allocatedmem) peakmem = allocatedmem;

	if (this->mutex) while (!this->mutex);
	this->mutex++;

	{
		sprintf_s(buffer, "%s: %d bytes @0X%08x at file %s line %d. Sum: %d bytes\n\0", msg, size, ptr, file, line, allocatedmem);
		fputs(buffer, fp);
		fflush(fp);
	}
	this->mutex--;
}

void MallocTrk::add(const char *msg, unsigned int size, const char *file, int line, void *ptr) {
	// --- 
	memory_token_t tmp;

	strcpy_s(tmp.filename, file);
	strcpy_s(tmp.comment, msg);
	tmp.line = line;
	tmp.size = size;

	memoryMap[ptr] = tmp;

	// ---
	sumalloc += size;
	allocatedmem += size;

	opcount++;
	
	memorylog(msg, size, file, line, ptr);
}

void MallocTrk::remove(const char *msg, const char *file, int line, void *ptr) {
	if (memoryMap.empty())
		return;

	int keres = memoryMap.count(ptr);
	if (memoryMap.count(ptr) == 0)
		return;

	memory_map_t::iterator it = memoryMap.find(ptr);
	memory_token_t tmp = it->second;

	allocatedmem -= tmp.size;

	opcount++;

	memoryMap.erase(it);
	memorylog(msg, tmp.size, file, line, ptr);

}

void MallocTrk::resize(const char *msg, int newsize, const char *file, int line, void *ptr, void *newptr) {
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

	opcount++;

	// csere
	strcpy_s(tmp.filename, file);
	strcpy_s(tmp.comment, msg);
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


void MallocTrk::write_allocationTable() {
	FILE* fp_ = NULL;
	if (fopen_s(&fp_, MALLOC_TRK_LEAK_FILENAME, "wb")) throw 2;
	if (!fp_) throw 1;

	int mem = 0;

	fprintf_s(fp_, "Peak memory usage: %d bytes \n", peakmem);
	fprintf_s(fp_, "Allocation: %d bytes \n", sumalloc);
	fprintf_s(fp_, "Number of memory operations: %d \n", opcount);
	fprintf_s(fp_, "\n---------------------------------------------------------\n");

	if (memoryMap.empty()) {
		fprintf_s(fp_, "No memory leak detected, you are awesome, yey :3\n");

	}
	else {
		for (memory_map_t::iterator it = memoryMap.begin(); it != memoryMap.end(); ++it) {
			void * ptr = it->first;
			memory_token_t &tmp = it->second;

			mem += tmp.size;

			fprintf_s(fp_, "%s: %d bytes @0X%08x at file %s line %d. Sum: %d bytes\n",
				tmp.comment, tmp.size, ptr, tmp.filename, tmp.line, mem);
		}
	}

	fflush(fp_);
	fclose(fp_);
}