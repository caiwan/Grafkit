#define NO_NEW_OVERRIDE
#include "stdafx.h"
#include "memory.h"
#undef NO_NEW_OVERRIDE

#include <cstdlib>

#ifndef LIVE_RELEASE

/**/
track_printer::~track_printer() 
{
	FILE* fp = nullptr;
	fopen_s(&fp, "memory.log", "wb");
	if (fp) {
		auto it = track->begin();
		while (it != track->end()) {
			const track_record &tr = it->second;
			fprintf_s(fp, "%d bytes at %s %s line %d\n", tr.size, tr.file.c_str(), tr.function.c_str(), tr.line);
			++it;
		}
		fflush(fp); fclose(fp);
	}
}


track_type * get_map() {
	static track_type * track = new (std::malloc(sizeof *track)) track_type;
	static track_printer printer(track);
	return track;
}

void add_alloc(void * mem, std::size_t size, const char * file, const char * function, unsigned int line)
{
	(*get_map())[mem] = track_record(size, file, function, line);
}

void remove_alloc(void * mem)
{
	if (get_map()->erase(mem) == 0) {
		// this indicates a serious bug
		// Grafkit::Log::Logger().Error("bug: mem at %d, bytes wasn't allocated by us", mem);
	}
}

/**/
void * __cdecl operator new(size_t size, const char* file, const char* function, unsigned int line) throw(std::bad_alloc) {
	// we are required to return non-null
	void * mem = std::malloc(size == 0 ? 1 : size);
	if (mem == 0) {
		throw std::bad_alloc();
	}
	add_alloc(mem, size, file, function, line);
	return mem;
}

void __cdecl operator delete(void * mem) throw() {
	remove_alloc(mem);
	std::free(mem);
}

#endif //LIVE_RELEASE
