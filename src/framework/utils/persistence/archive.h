#ifndef __ARCHIVE_H_
#define __ARCHIVE_H_

#include "persistence.h"

namespace Grafkit{ 

	/**
		Fileba ment/filebol tolt.
	*/
	class ArchiveFile: public Archive
	{
		public:
			ArchiveFile(FILE* stream, bool IsStoring = false);
			virtual ~ArchiveFile();

	    void Write(const void *buffer, size_t length) override;
	    void Read (void* buffer, size_t length) override;
	private:
		FILE * _stream;
	};

	/*
		Memoriabol tolt be. Csak olvashato.
	*/
	class ArchiveMemory : public Archive
	{

	public:
		ArchiveMemory(BYTE* data, size_t length, bool IsStoring = false);
		virtual ~ArchiveMemory();

	    void Write(const void *buffer, size_t length) override {}
	    void Read(void* buffer, size_t length) override;

	private:
		BYTE* m_data;
		size_t m_length, m_cursor;
	};


};

#endif //__ARCHIVE_H_