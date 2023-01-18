#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/persistence/dynamics.h"

#include "testClass_dynamics.h"

using namespace Grafkit;

#define CLONEABLE_TestClassName "_dummy_test_class"

// ======
template <typename T>
void TEST_genclass(const char* name) {
	const Clonable *item = Clonables::Instance().Find(name);
	ASSERT_TRUE(item != NULL);

	// generating new item 
	Clonable *elem = Clonables::Instance().Create(name);
	ASSERT_TRUE(elem != NULL);
	ASSERT_TRUE(elem != item);
	ASSERT_TRUE(dynamic_cast<T*>(elem) != NULL);

	delete elem;
}

// ======
#define CLONEABLE_TestCloneableFactoryName "_dummy_test_clazz_factory_clonable"
static AddClonable _addClonable_2(CLONEABLE_TestCloneableFactoryName, new ArchiveFactoryTestClass::Factory());

TEST(Dynamics, given_NoCloable_when_AddFactory_then_GetFactory) {
	TEST_genclass<ArchiveFactoryTestClass>(CLONEABLE_TestCloneableFactoryName);
}

/*
	Test if clonables could be added within the module
*/

TEST(Dynamics, given_NoFactory_when_ModuleLevelAddClonableFactory_thenGetFactory) {
	TEST_genclass<ArchiveFactoryTestClass>("ArchiveFactoryTestClass");
}
