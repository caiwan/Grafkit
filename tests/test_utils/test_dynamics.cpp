#include "stdafx.h"

#include "gtest/gtest.h"

#include "utils/persistence/dynamics.h"

#include "testClass_dynamics.h"


using namespace Grafkit;

#define CLONEABLE_TestClassName "_dummy_test_clazz"

/* Obsolete; we're using bean factories from now on */
TEST(Dynamics, NotFind_Add_Find) {
	
	// item is not added yet 
	const Clonable *item0 = Clonables::Instance().find(CLONEABLE_TestClassName);
	ASSERT_TRUE(item0 == NULL);

	// add the new item
	ArchiveClonableTestClass* testclazz = new ArchiveClonableTestClass();
	Clonables::Instance().addClonable(CLONEABLE_TestClassName, testclazz);

	// item is added; finding it 
	const Clonable *item1 = Clonables::Instance().find(CLONEABLE_TestClassName);
	ASSERT_TRUE(item1 != NULL);

	// generating new item 
	Clonable *elem = Clonables::Instance().create(CLONEABLE_TestClassName);
	ASSERT_TRUE(elem != NULL);
	ASSERT_TRUE(elem != item0);
	ASSERT_TRUE(dynamic_cast<ArchiveClonableTestClass*>(elem) != NULL);

	delete elem;
}

// ======
template <typename T> inline void TEST_genclass(const char* name) {
	const Clonable *item = Clonables::Instance().find(name);
	ASSERT_TRUE(item != NULL);

	// generating new item 
	Clonable *elem = Clonables::Instance().create(name);
	ASSERT_TRUE(elem != NULL);
	ASSERT_TRUE(elem != item);
	ASSERT_TRUE(dynamic_cast<T*>(elem) != NULL);

	delete elem;
}

#define CLONEABLE_TestCloneableName "_dummy_test_clazz_static_clonable"
static Grafkit::AddClonable _addClonable_1(CLONEABLE_TestCloneableName, new ArchiveClonableTestClass());

/* Obsolete; we're using bean factories from now on */
TEST(Dynamics, StaticAddCloneable) {
	TEST_genclass<ArchiveClonableTestClass>(CLONEABLE_TestCloneableName);
}

// ======
#define CLONEABLE_TestCloneableFactoryName "_dummy_test_clazz_factory_clonable"
static Grafkit::AddClonable _addClonable_2(CLONEABLE_TestCloneableFactoryName, new ArchiveFactoryTestClass::Factory());

TEST(Dynamics, Factory) {
	TEST_genclass<ArchiveFactoryTestClass>(CLONEABLE_TestCloneableFactoryName);
}

/*
	Test if clonables could be added within the module
*/
TEST(Dynamics, ModuleLevelAddClonable) {
	TEST_genclass<ArchiveClonableTestClass>("ArchiveClonableTestClass");
}

TEST(Dynamics, ModuleLevelAddClonableFactory) {
	TEST_genclass<ArchiveFactoryTestClass>("ArchiveFactoryTestClass");
}