#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/asset/AssetFile.h"
#include "scene/scene.h"

#include "core/system.h"

#include "render/material.h"
#include "render/texture.h""

#include "resource/loaders/TextureLoader.h"

#include "application.h"

// --- 

using namespace Grafkit;

#define ASSET_ROOT "tests/assets/"
#define TEXTURE_ROOT "textures/"

class TextureLoadTest : public testing::Test
{
public:
    TextureLoadTest() : m_app(nullptr)
    {
        m_app = std::make_unique<Testing::TestApplicationContext>(
            m_render, std::make_unique<FileAssetFactory>(ASSET_ROOT)
        );
    }

    ~TextureLoadTest() {
    }

    void SetUp() override {
    }

    void TearDown() override {
    }

protected:
    std::unique_ptr<Testing::TestApplicationContext> m_app;
    Renderer m_render;
};

//
#define TEXTURE_2D_UUID Uuid("54484011-5c9e-4479-81f4-937888131e60")
#define TEXTURE_NAME "normap.jpg"
#define TEXTURE_PATH TEXTURE_ROOT TEXTURE_NAME

TEST_F(TextureLoadTest, Load2DTexture)
{
    // given
    auto& resourceManager = m_app->GetResourceManager();
    Texture2DRes resource = resourceManager.Load<Texture2D>(
        std::make_shared<TextureFromBitmap>(TEXTURE_2D_UUID, TextureBitmapParams({TEXTURE_NAME, TEXTURE_PATH}))
    );

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    resourceManager.DoPrecalc(m_app->GetRender(), m_app->GetAssetFactory());

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ASSERT_STREQ(TEXTURE_2D_UUID.c_str(), resource.GetId().c_str());

    ASSERT_NE(nullptr, resource->GetTexture2D().Get());
    ASSERT_NE(nullptr, resource->GetTextureResource().Get());
    ASSERT_NE(nullptr, resource->GetShaderResourceView().Get());
}

namespace
{
    const char* cubemapNames[] = {
        "yoko_posx.jpg",
        "yoko_negx.jpg",
        "yoko_posy.jpg",
        "yoko_negy.jpg",
        "yoko_posz.jpg",
        "yoko_negz.jpg",
    };
}

#define TEXTURE_CUBE_UUID Uuid("af6404fa-a4f7-4b01-8c8b-1218faf6d35c")
//
TEST_F(TextureLoadTest, LoadCubeMap)
{
    // given
    auto& resourceManager = m_app->GetResourceManager();

    TextureCubemapParams params{"cubeMap", {}};
    std::transform(cubemapNames, cubemapNames + 6, params.sourceNames.begin()/*back_inserter(params.sourceNames)*/, [](std::string s)-> std::string { return std::string(TEXTURE_ROOT) + s; });

    TextureCubeRes resource = resourceManager.Load<TextureCube>(
        std::make_shared<TextureCubemapFromBitmap>(TEXTURE_CUBE_UUID, params)
    );

    ASSERT_TRUE(!resource.Empty());
    ASSERT_TRUE(resource.Valid());

    // when
    resourceManager.DoPrecalc(m_app->GetRender(), m_app->GetAssetFactory());

    // then
    ASSERT_TRUE(!resource.Empty());
    ASSERT_TRUE(resource.Valid());

    ASSERT_STREQ(TEXTURE_CUBE_UUID.c_str(), resource.GetId().c_str());

    ASSERT_NE(nullptr, resource->GetTexture2D().Get());
    ASSERT_NE(nullptr, resource->GetTextureResource().Get());
    ASSERT_NE(nullptr, resource->GetShaderResourceView().Get());
}

#define TEXTURE_NOISE_UUID Uuid("91bde168-b40e-4c81-b974-2977014e5460")

//
TEST_F(TextureLoadTest, LoadNoiseMap)
{
    // given
    auto& resourceManager = m_app->GetResourceManager();

    Texture2DResRef resource = resourceManager.Load<Texture2D>(
        std::make_shared<TextureNoiseMapBuilder>(TEXTURE_NOISE_UUID, TextureNoiseParams{"noiseMap", 256})
    );

    ASSERT_TRUE(!resource.Empty());
    ASSERT_TRUE(resource.Valid());

    // when
    resourceManager.DoPrecalc(m_app->GetRender(), m_app->GetAssetFactory());


    // then
    ASSERT_TRUE(!resource.Empty());
    ASSERT_TRUE(resource.Valid());

    ASSERT_STREQ(TEXTURE_NOISE_UUID.c_str(), resource.GetId().c_str());

    ASSERT_NE(nullptr, resource->GetTexture2D().Get());
    ASSERT_NE(nullptr, resource->GetTextureResource().Get());
    ASSERT_NE(nullptr, resource->GetShaderResourceView().Get());
}
