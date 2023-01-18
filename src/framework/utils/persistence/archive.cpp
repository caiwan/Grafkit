#include "stdafx.h"

#include "archive.h"

/**
	ArchiveFile
*/

using namespace FWdebugExceptions;
using namespace Grafkit;

ArchiveFile::ArchiveFile(FILE* stream, bool IsStoring) : 
	Archive(IsStoring), 
	_stream(stream)
{
	if(!_stream) throw new EX(NullPointerException);
}

ArchiveFile::~ArchiveFile() {

}

void ArchiveFile::Write(const void* buffer, size_t length)
{
	if(!_stream) throw new EX(NullPointerException);
	
	//Log::Logger().Debug("Write bytes %d at pos %d", length, ftell(_stream));

	fwrite(buffer, length, 1, this->_stream);
}

void ArchiveFile::Read(void* buffer, size_t length)
{
	if(!_stream) throw new EX(NullPointerException);

	//Log::Logger().Debug("Read bytes %d at pos %d", length, ftell(_stream));

	fread(buffer, length, 1, this->_stream);
}

/* 

*/

Grafkit::ArchiveMemory::ArchiveMemory(BYTE * data, size_t length, bool IsStoring) :
	m_data(data), m_length(length), m_cursor(0)
{
}

Grafkit::ArchiveMemory::~ArchiveMemory()
{
}

void Grafkit::ArchiveMemory::Read(void * buffer, size_t length)
{
	if (length + m_cursor > m_length) {
#ifdef _DEBUG
		DebugBreak();
#endif
		throw new EX(OutOfBoundsException);
	}
	//Log::Logger().Debug("Read bytes %d at pos %d", length, m_cursor);

	memcpy_s(buffer, length, &m_data[m_cursor], length);
	m_cursor += length;
}
