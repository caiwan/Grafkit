#include "stdafx.h"
#include <gtest/gtest.h>
#include "utils/asset/AssetFile.h"

#include "application.h"
#include "dummyResources.h"

// --- 

using namespace Grafkit;

#define ASSET_ROOT "tests/assets/"

class PreloaderTest : public testing::Test
{
public:
    PreloaderTest() : m_app(nullptr)
    {
        m_assetFactory = new FileAssetFactory(ASSET_ROOT);
        m_app = new Testing::TestApplicationContext(m_render, m_assetFactory);
    }

    ~PreloaderTest()
    {
        delete m_assetFactory;
        delete m_app;
    }

    void SetUp() override {
    }

    void TearDown() override
    {
    }
 
protected:
    Testing::TestApplicationContext* m_app;

    Renderer m_render;
    IAssetFactory* m_assetFactory;

};

#define DUMMY_UUID "dummy_uuid"
#define DUMMY_NAME "dummy_name"

//
TEST_F(PreloaderTest, RegisterAndLoadObject)
{

    // given
    DummyResourceParameters params{ "lol", 2, 3 };
    m_app->RegisterBuilderFactory(new ResourceBuilderFactroy<DummyObjectLoader>("dummyObject"));

    // when
    m_app->PushNewBuilder("dummyObject", DUMMY_NAME ,DUMMY_UUID, &params);
    m_app->DoPrecalc();

    // then
    Ref<Resource<DummyObject>> loadedObject = m_app->GetByUuid<Resource<DummyObject>>(DUMMY_UUID);
    ASSERT_TRUE(loadedObject);
    ASSERT_TRUE(*loadedObject);

    ASSERT_FLOAT_EQ(params.param1, (*loadedObject)->GetParam1());
    ASSERT_FLOAT_EQ(params.param2, (*loadedObject)->GetParam2());

    ASSERT_TRUE((*loadedObject)->GetIsInitialized());
}

TEST_F(PreloaderTest, ChangeParametersThenReload)
{
    FAIL() << "Not implemented";
}

TEST_F(PreloaderTest, ChangeParametersThenUndo)
{
    FAIL() << "Not implemented";
}