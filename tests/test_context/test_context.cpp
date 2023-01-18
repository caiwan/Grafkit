#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/AssetFile.h"
#include "demo.h"
#include "context.h"
#include "schema.h"

// --- 

using namespace GkDemo;

#define JSON_PATH "schema.json"

class ContextPregnancyTest : public testing::Test
{
public:
    ContextPregnancyTest(): m_context(nullptr)
        , m_assetFactory(nullptr) {
        m_assetFactory = new Grafkit::FileAssetFactory("tests/assets/");
    }

    void SetUp() override {
        m_context = new Context(m_render, m_assetFactory);
    }

    void TearDown() override {
    }

    void BuildDemo()
    {
        try
        {
            SchemaBuilder builder;
            builder.LoadFromAsset(m_assetFactory->Get(JSON_PATH), dynamic_cast<Grafkit::IResourceManager*>(m_context));
            m_demo = builder.GetDemo();
        }
        catch (std::exception& e)
        {
            FAIL() << e.what();
        }
    }

private:
    Ref<Demo> m_demo; 
    Context *m_context;
    Grafkit::Renderer m_render;
    Grafkit::IAssetFactory *m_assetFactory;
};

TEST_F(ContextPregnancyTest, TestJsonLoad)
{
    // given
    this->BuildDemo();

    // when

    // then
}
