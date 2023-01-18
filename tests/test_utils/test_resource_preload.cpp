/**
Unittest for resource management system II.
*/

#include "stdafx.h"

#include <gtest/gtest.h>

#include "testClass_resource.h"
// --- 

class ResourcePreloaderTest : public testing::Test {

public:
    void SetUp() override {
    }

    void TearDown() override {
    }
};



TEST_F(ResourcePreloaderTest, TestAssignment) {
    ThingResourceRef resource = new ThingResource();

    resource->AssingnRef(new Thing());

    ASSERT_TRUE(resource.Valid() && resource->Valid());
}
