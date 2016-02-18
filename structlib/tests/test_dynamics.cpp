#include "gtest/gtest.h"

#include "testClass_dynamics.h"
#include "dynamics.h"

using namespace FWutils;

#define CLONEABLE_TestClassName "_dummy_test_clazz"

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

#define CLONEABLE_TestCloneableName "_dummy_test_clazz_static_clonable"
static FWutils::AddClonable _addClonable(CLONEABLE_TestCloneableName, new ArchiveClonableTestClass());

TEST(Dynamics, StaticAddCloneable) {
	const Clonable *item = Clonables::Instance().find(CLONEABLE_TestCloneableName);
	ASSERT_TRUE(item != NULL);

	// generating new item 
	Clonable *elem = Clonables::Instance().create(CLONEABLE_TestCloneableName);
	ASSERT_TRUE(elem != NULL);
	ASSERT_TRUE(elem != item);
	ASSERT_TRUE(dynamic_cast<ArchiveClonableTestClass*>(elem) != NULL);

	delete elem;
}

// ======

TEST(Dynamics, Factory) {

}