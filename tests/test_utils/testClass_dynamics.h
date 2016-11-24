#pragma once 

#include "utils/persistence/dynamics.h"

class ArchiveFactoryTestClass : public Grafkit::Clonable {
public:
	ArchiveFactoryTestClass() {}

public:
	//CLONEABLE_DECL(ArchiveFactoryTestClass);
	CLONEABLE_FACTORY_DECL(ArchiveFactoryTestClass);
};
