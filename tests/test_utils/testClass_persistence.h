#pragma once

#include "utils/persistence/persistence.h"

class ArchivePersistentTestEmptyClass : public Grafkit::Persistent {
	PERSISTENT_DECL(ArchivePersistentTestEmptyClass, 1)
public:
	ArchivePersistentTestEmptyClass() {}
	~ArchivePersistentTestEmptyClass() {}
};
