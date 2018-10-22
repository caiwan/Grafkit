#include "stdafx.h"

#include "archive.h"

/**
	ArchiveFile
*/

using namespace FWdebugExceptions;
using namespace Grafkit;

#if 0

ArchiveFile::ArchiveFile(FILE* stream, bool IsStoring) : 
	Archive(IsStoring), 
	m_stream(stream)
{
	if(!m_stream) THROW_EX(NullPointerException);
}

ArchiveFile::~ArchiveFile() {

}

void ArchiveFile::Write(const void* buffer, size_t length)
{
	if(!m_stream) THROW_EX(NullPointerException);
	fwrite(buffer, length, 1, this->m_stream);
}

void ArchiveFile::Read(void* buffer, size_t length)
{
	if(!m_stream) THROW_EX(NullPointerException);
	fread(buffer, length, 1, this->m_stream);
}

/* 
 * 
*/

ArchiveMemory::ArchiveMemory(BYTE * data, size_t length, bool IsStoring) :
	m_data(data), m_length(length), m_cursor(0)
{
}

ArchiveMemory::~ArchiveMemory()
{
}

void ArchiveMemory::Write(const void* buffer, size_t length) {
    THROW_EX_DETAILS(NotImplementedMethodException, "");
}

void ArchiveMemory::Read(void * buffer, size_t length)
{
	if (length + m_cursor > m_length) {
#ifdef _DEBUG
		DebugBreak();
#endif
		THROW_EX(OutOfBoundsException);
	}
	memcpy_s(buffer, length, &m_data[m_cursor], length);
	m_cursor += length;
}

/*
 *
*/

ArchiveAsset::ArchiveAsset(const StreamRef& asset): Archive(false),
m_asset(asset), m_cursor(0) {
}

void ArchiveAsset::Write(const void* buffer, size_t length) {
    THROW_EX(NotImplementedMethodException);
}

void ArchiveAsset::Read(void* buffer, size_t length) {
    if (length + m_cursor > m_asset->GetSize()) {
#ifdef _DEBUG
        DebugBreak();
#endif
        THROW_EX(OutOfBoundsException);
    }
    uint8_t* data = static_cast<uint8_t*>(m_asset->GetData());
    memcpy_s(buffer, length, &data[m_cursor], length);
    m_cursor += length;
}

#endif
