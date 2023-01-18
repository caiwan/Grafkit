#pragma once
#include "core/exceptions.h"
#include "utils/persistence/persistence.h"

// ----------------------------------------
class TestArchiver : public Grafkit::Archive
{
private:
	unsigned char *data;
	size_t crsr, size;
	int no_delete;

public:
	TestArchiver(size_t initialsize = 256, bool isStoring = false);
	TestArchiver(const void* in_data, size_t size);
	~TestArchiver();

	virtual void Write(const void* buffer, size_t length);
	virtual void Read(void* buffer, size_t length);

	unsigned char*& GetBuffer() { return this->data; }
	size_t GetSize() { return this->size; }
	size_t GetCrsr() { return this->crsr; }

	void ResetCrsr() { this->crsr = 0; }

	void Hexdump();
};

