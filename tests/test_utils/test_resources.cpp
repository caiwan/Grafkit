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

    resource->SetUuid("theThing");

    ASSERT_TRUE(resource.Valid() && resource->Valid());

    resman->Add(resource);

    ThingResourceRef retresource = resman->GetByUuid<ThingResource>("theThing");

    ASSERT_TRUE(retresource.Valid() && retresource->Valid());

    ASSERT_TRUE(retresource->Get() == thing);

    delete resman;
}

TEST_F(ResourceManagerTest, TestReplace) {
    MyResourceManager *resman = new MyResourceManager();

    ThingResourceRef resource = new ThingResource();
    resource->AssingnRef(new Thing());

    resource->SetUuid("theThing");

    resman->Add(resource);

    ThingResourceRef retresource = resman->GetByUuid<ThingResource>("theThing");
    ASSERT_TRUE(retresource.Valid() && retresource->Valid());

    ThingRef newThing = new Thing();
    retresource->AssingnRef(newThing);

    ASSERT_TRUE(retresource->Get() == newThing);
    ASSERT_TRUE(resource->Get() == newThing);

    delete resman;
}


TEST_F(ResourceManagerTest, TestRemove) {
    MyResourceManager *resman = new MyResourceManager();

    ThingResourceRef resource = new ThingResource();
    resource->AssingnRef(new Thing());

    resource->SetUuid("theThing");

    ASSERT_TRUE(resource.Valid() && resource->Valid());

    resman->Add(resource);

    ThingResourceRef retresource;

    retresource = resman->GetByUuid<ThingResource>("theThing");
    ASSERT_TRUE(retresource.Valid() && retresource->Valid());

    resman->RemoveByUuid("theThing");

    retresource = resman->GetByUuid<ThingResource>("theThing");
    ASSERT_TRUE(retresource.Invalid());

    delete resman;
}

TEST_F(ResourceManagerTest, TestLoad) {
    // given 
    ThingResourceRef resource;
    MyResourceManager *resman = new MyResourceManager();


    resman->Load(new ThingLoader("theLoadedThing"));

    resource = resman->GetByUuid<ThingResource>("theLoadedThing");

    ASSERT_TRUE(resource.Valid());
    ASSERT_TRUE(resource->Invalid());

    // when
    resman->DoPrecalc();

    // then
    // it has to be updated without refresh
    ASSERT_TRUE(resource.Valid());
    ASSERT_TRUE(resource->Valid());

    resource = resman->GetByUuid<ThingResource>("theLoadedThing");

    ASSERT_TRUE(resource.Valid());
    ASSERT_TRUE(resource->Valid());

    delete resman;
}


TEST_F(ResourceManagerTest, TestCasting) {
    // given
    MyResourceManager *resman = new MyResourceManager();

    // when
    resman->Add(new AnotherThingResource(new AnotherThing(), "anotherThing"));

    ThingResourceRef resource = resman->GetByUuid<ThingResource>("anotherThing");

    // then
    ASSERT_FALSE(resource.Valid());

    delete resman;
}

TEST_F(ResourceManagerTest, ReloadTest) {
    // given
    MyResourceManager *resman = new MyResourceManager();
    ThingResourceRef resource = resman->Load<ThingResource>(new ThingLoader("theLoadedThing"));

    ASSERT_TRUE(resource.Valid());
    ASSERT_TRUE(resource->Invalid());

    resman->DoPrecalc();

    ASSERT_TRUE(resource->Valid());

    (*resource)->SetAsd(999);
    (*resource)->SetGlejd(999.);

    //when
    resman->TriggerReload("theLoadedThing");

    //then
    ASSERT_TRUE(resource.Valid());
    ASSERT_TRUE(resource->Valid());

    ASSERT_EQ(0x010101, (*resource)->GetAsd());
    ASSERT_DOUBLE_EQ(2.16f, (*resource)->GetGlejd());

}