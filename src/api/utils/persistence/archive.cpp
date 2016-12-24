#include "stdafx.h"

#include "archive.h"

/**
	ArchiveFile
*/

using namespace FWdebugExceptions;
using namespace Grafkit;

ArchiveFile::ArchiveFile(FILE* stream, bool isStoring) : 
	Archive(isStoring), 
	_stream(stream)
{
	if(!_stream) throw new EX(NullPointerException);
}

ArchiveFile::~ArchiveFile() {

}

void ArchiveFile::write(const void* buffer, size_t length)
{
	if(!_stream) throw new EX(NullPointerException);
	
	Log::Logger().Debug("Write bytes %d at pos %d", length, ftell(_stream));

	fwrite(buffer, length, 1, this->_stream);
}

void ArchiveFile::read(void* buffer, size_t length)
{
	if(!_stream) throw new EX(NullPointerException);

	Log::Logger().Debug("Read bytes %d at pos %d", length, ftell(_stream));

	fread(buffer, length, 1, this->_stream);
}