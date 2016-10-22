#include "stdafx.h"

#include "gtest/gtest.h"

#include "utils/persistence/dynamics.h"

#include "testClass_dynamics.h"


using namespace Grafkit;

#define CLONEABLE_TestClassName "_dummy_test_clazz"

/* Obsolete; we're using bean factories from now on */
TEST(Dynamics, given_NoClonable_when_Add_then_Getclonable) {
	
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
TEST(Dynamics, given_NoClonable_when_CallStaticAddCloneable_then_GetClonable) {
	TEST_genclass<ArchiveClonableTestClass>(CLONEABLE_TestCloneableName);
}

// ======
#define CLONEABLE_TestCloneableFactoryName "_dummy_test_clazz_factory_clonable"
static Grafkit::AddClonable _addClonable_2(CLONEABLE_TestCloneableFactoryName, new ArchiveFactoryTestClass::Factory());

TEST(Dynamics, given_NoCloable_when_AddFactory_then_GetFactory) {
	TEST_genclass<ArchiveFactoryTestClass>(CLONEABLE_TestCloneableFactoryName);
}

/*
	Test if clonables could be added within the module
*/
TEST(Dynamics, given_NoClonable_when_ModuleLevelAddClonable_then_GetClonable) {
	TEST_genclass<ArchiveClonableTestClass>("ArchiveClonableTestClass");
}

TEST(Dynamics, given_NoFactory_when_ModuleLevelAddClonableFactory_thenGetFactory) {
	TEST_genclass<ArchiveFactoryTestClass>("ArchiveFactoryTestClass");
}
