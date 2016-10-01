#ifndef __ARCHIVE_H_
#define __ARCHIVE_H_

#include "dynamics.h"
#include "persistence.h"

namespace Grafkit{ 

	/*
		A base Archive a persistent.h-ban van
	*/

	/**
		Fileba ment/filebol tolt.
	*/
	class ArchiveFile: public Archive
	{
		private:
			FILE * _stream;

		public:
			ArchiveFile(FILE* stream, bool isStoring = false);
			virtual ~ArchiveFile();

			virtual void write(const void *buffer, size_t length);  
			virtual void read (void* buffer, size_t length);
	};

	// tovabbi IO stuff goez here

};

#endif //__ARCHIVE_H_