#pragma once 

#include "dynamics.h"

class ArchiveClonableTestClass : public FWutils::Clonable {
public:
	ArchiveClonableTestClass() {}

public:
	CLONEABLE_DECL(ArchiveClonableTestClass);
};

// ================================================================================================

class ArchiveFactoryTestClass : public FWutils::Clonable {
public:
	ArchiveFactoryTestClass() {}

public:
	CLONEABLE_DECL(ArchiveFactoryTestClass);
	CLONEABLE_FACTORY_DECL(ArchiveFactoryTestClass);
};
