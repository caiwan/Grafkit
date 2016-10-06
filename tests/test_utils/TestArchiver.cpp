#include "stdafx.h"

#include "TestArchiver.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

TestArchiver::TestArchiver(size_t initsize, bool isStoring) : Archive(isStoring),
	size(0),
	data(NULL),
	crsr(0),
	no_delete(0)
{
	this->size = initsize;
	this->data = new unsigned char[this->size];
}

TestArchiver::TestArchiver(const void * in_data, size_t in_size): Archive(false),
	size(in_size),
	data((unsigned char*)in_data),
	crsr(0),
	no_delete(1)
{
}


TestArchiver::~TestArchiver()
{
	if (!no_delete)
		delete[]this->data;
}

void TestArchiver::write(const void* buffer, size_t length)
{
	if (this->crsr + length >= this->size) throw new EX(OutOfBoundsException);
	unsigned char *dst = &this->data[this->crsr];
	const unsigned char *src = (const unsigned char*)buffer;
	for (size_t i = 0; i < length; ++i)
	{
		dst[i] = src[i];
	}
	this->crsr += length;
}

void TestArchiver::read(void* buffer, size_t length)
{
	if (this->crsr + length >= this->size) throw new EX(OutOfBoundsException);
	unsigned char *src = &this->data[this->crsr];
	unsigned char *dst = (unsigned char*)buffer;
	for (size_t i = 0; i < length; ++i)
	{
		dst[i] = src[i];
	}
	this->crsr += length;
}

void TestArchiver::hexdump()
{
	printf_s("Dump data:\n");
	for (size_t i = 0; i < this->size; ++i) {
		printf_s("%02x ", this->data[i]);
		if (i && i % 15 == 0) {
			printf_s("\n");
		}
	}
	printf_s("\n");
}
