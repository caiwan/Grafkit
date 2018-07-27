#include "stdafx.h"
#include <gtest/gtest.h>

#include "context.h"
#include "schema.h"

// --- 

using namespace GkDemo;

#define JSON_PATH "../assets/schema.json"

class ContextPregnancyTest : public testing::Test {

public:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(ContextPregnancyTest, TestJsonLoad) {
    SchemaBuilder builder;
    builder.LoadFromFile(JSON_PATH);

}
