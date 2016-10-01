#include "archive.h"
#include "../exceptions.h"
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
	this->initCheck();
}

ArchiveFile::~ArchiveFile() {

}

void ArchiveFile::write(const void* buffer, size_t length)
{
	if(!_stream) throw new EX(NullPointerException);

	fwrite(buffer, length, 1, this->_stream);
	/*if(!*_stream)
		throw "ArchiveFile::write Error";
	*/

	///@todo hibakezeles
}

void ArchiveFile::read(void* buffer, size_t length)
{
	if(!_stream) throw new EX(NullPointerException);

	fread(buffer, length, 1, this->_stream);
	
	/*if(! *_stream)
		throw "ArchiveFile::read Error";    
		*/

	///@todo hibakezeles
}