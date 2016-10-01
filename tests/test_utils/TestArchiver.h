#pragma once
#include "utils/exceptions.h"
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

	virtual void write(const void* buffer, size_t length);
	virtual void read(void* buffer, size_t length);

	unsigned char*& getBuffer() { return this->data; }
	size_t getSize() { return this->size; }
	size_t getCrsr() { return this->crsr; }

	void resetCrsr() { this->crsr = 0; }
};

