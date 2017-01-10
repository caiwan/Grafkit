#ifndef __ARCHIVE_H_
#define __ARCHIVE_H_

#include "dynamics.h"
#include "persistence.h"

namespace Grafkit{ 

	/**
		Fileba ment/filebol tolt.
	*/
	class ArchiveFile: public Archive
	{
		public:
			ArchiveFile(FILE* stream, bool isStoring = false);
			virtual ~ArchiveFile();

			virtual void write(const void *buffer, size_t length);  
			virtual void read (void* buffer, size_t length);
	private:
		FILE * _stream;
	};

	/*
		Memoriabol tolt be. Csak olvashato.
	*/
	class ArchiveMemory : public Archive
	{

	public:
		ArchiveMemory(BYTE* data, size_t length, bool isStoring = false);
		virtual ~ArchiveMemory();

		virtual void write(const void *buffer, size_t length) {}	
		virtual void read(void* buffer, size_t length);

	private:
		BYTE* m_data;
		size_t m_length, m_cursor;
	};


};

#endif //__ARCHIVE_H_