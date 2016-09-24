#pragma once 

#include "utils/dynamics.h"

class ArchiveClonableTestClass : public Grafkit::Clonable {
public:
	ArchiveClonableTestClass() {}

public:
	CLONEABLE_DECL(ArchiveClonableTestClass);
};

// ================================================================================================

class ArchiveFactoryTestClass : public Grafkit::Clonable {
public:
	ArchiveFactoryTestClass() {}

public:
	CLONEABLE_DECL(ArchiveFactoryTestClass);
	CLONEABLE_FACTORY_DECL(ArchiveFactoryTestClass);
};
