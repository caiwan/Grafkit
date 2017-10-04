/**
	Unittest for resource management system 
*/

#include "stdafx.h"

#include <gtest/gtest.h>

#include "testClass_resource.h"
// --- 

class ResourceManagerTest : public testing::Test {

public:
	void SetUp() {
	}
	
	void TearDown() {
	}   
};


TEST_F(ResourceManagerTest, given_EmptyRes_when_Assign_then_Assigned) {
	ThingResourceRef resource = new ThingResource();

	resource->AssingnRef(new Thing());

	ASSERT_TRUE(resource.Valid() && resource->Valid());
}

TEST_F(ResourceManagerTest, given_ResourceManager_when_AddResource_then_GetResource) {
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

TEST_F(ResourceManagerTest, given_Resource_when_ReplaceInManager_then_GetResource) {
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


TEST_F(ResourceManagerTest, given_Resource_when_RemoveFromManager_then_GetNull ) {
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

TEST_F(ResourceManagerTest, given_ResourceBuilder_when_LoadResource_then_GetResource) {
	ThingResourceRef resource;
	MyResourceManager *resman = new MyResourceManager();

	resman->Load(new ThingLoader("theLoadedThing"));

	resource = resman->Get<ThingResource>("theLoadedThing");

	ASSERT_TRUE(resource.Valid());
	ASSERT_TRUE(resource->Invalid());

	resman->DoPrecalc();

	// lekeres nelkul is frissulnie kell
	ASSERT_TRUE(resource.Valid());
	ASSERT_TRUE(resource->Valid());

	resource = resman->Get<ThingResource>("theLoadedThing");

	ASSERT_TRUE(resource.Valid());
	ASSERT_TRUE(resource->Valid());

	delete resman;
}
