/**
	Unittest for resource management system 
*/

#include "stdafx.h"

#include <gtest/gtest.h>

#include "testClass_resource.h"
// --- 

class ResourceManagerTest : public testing::Test {

public:
	void SetUp() override {
	}

    void TearDown() override {
	}   
};


TEST_F(ResourceManagerTest, TestAssignment) {
	ThingResourceRef resource = new ThingResource();

	resource->AssingnRef(new Thing());

	ASSERT_TRUE(resource.Valid() && resource->Valid());
}

TEST_F(ResourceManagerTest, TestAddAndGet) {
	MyResourceManager *resman = new MyResourceManager();

	ThingResourceRef resource = new ThingResource();
	ThingRef thing = new Thing();
	resource->AssingnRef(thing);

	resource->SetName("theThing");

	ASSERT_TRUE(resource.Valid() && resource->Valid());
    
	resman->Add(resource);

	ThingResourceRef retresource = resman->Get<ThingResource>("theThing");

	ASSERT_TRUE(retresource.Valid() && retresource->Valid());

	ASSERT_TRUE(retresource->Get() == thing);

	delete resman;
}

TEST_F(ResourceManagerTest, TestReplace) {
	MyResourceManager *resman = new MyResourceManager();

	ThingResourceRef resource = new ThingResource();
	resource->AssingnRef(new Thing());

	resource->SetName("theThing");

	resman->Add(resource);

	ThingResourceRef retresource = resman->Get<ThingResource>("theThing");
	ASSERT_TRUE(retresource.Valid() && retresource->Valid());

	ThingRef newThing = new Thing();
	retresource->AssingnRef(newThing);

	ASSERT_TRUE(retresource->Get() == newThing);
	ASSERT_TRUE(resource->Get() == newThing);

	delete resman;
}


TEST_F(ResourceManagerTest, TestRemove ) {
	MyResourceManager *resman = new MyResourceManager();

	ThingResourceRef resource = new ThingResource();
	resource->AssingnRef(new Thing());

	resource->SetName("theThing");

	ASSERT_TRUE(resource.Valid() && resource->Valid());

	resman->Add(resource);

	ThingResourceRef retresource;
	
	retresource = resman->Get<ThingResource>("theThing");
	ASSERT_TRUE(retresource.Valid() && retresource->Valid());

	resman->Remove("theThing");

	retresource = resman->Get<ThingResource>("theThing");
	ASSERT_TRUE(retresource.Invalid());

	delete resman;
}

TEST_F(ResourceManagerTest, TestLoad) {
	ThingResourceRef resource;
	MyResourceManager *resman = new MyResourceManager();

	resman->Load(new ThingLoader("theLoadedThing"));

	resource = resman->Get<ThingResource>("theLoadedThing");

	ASSERT_TRUE(resource.Valid());
	ASSERT_TRUE(resource->Invalid());

	resman->DoPrecalc();

	// it has to be updated without refresh
	ASSERT_TRUE(resource.Valid());
	ASSERT_TRUE(resource->Valid());

	resource = resman->Get<ThingResource>("theLoadedThing");

	ASSERT_TRUE(resource.Valid());
	ASSERT_TRUE(resource->Valid());

	delete resman;
}


TEST_F(ResourceManagerTest, TestCasting) {
    // given
    MyResourceManager *resman = new MyResourceManager();

    // when
    resman->Add(new AnotherThingResource(new AnotherThing(), "anotherThing"));

    ThingResourceRef resource = resman->Get<ThingResource>("anotherThing");

    // then
    ASSERT_FALSE(resource.Valid());
 
    delete resman;
}