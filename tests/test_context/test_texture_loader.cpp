#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/asset/AssetFile.h"

#include "scene/scene.h"

#include "demo.h"
#include "context.h"
#include "schema.h"

#include "core/system.h"

#include "render/material.h"
#include "render/texture.h""

#include "resource/loadersTextureLoader.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define ASSET_ROOT "tests/assets/"
#define TEXTURE_ROOT "textures/"

namespace NSTextureLoadTest
{
    class TestApplicationWindow : public System
    {
    public:
        explicit TestApplicationWindow(Renderer& render) : m_render(render)
        {
            InitializeWindows(320, 240);
            m_render.Initialize(m_window.getRealWidth(), m_window.getRealHeight(), true, this->m_window.getHWnd(), false);
        }

        ~TestApplicationWindow() { ShutdownWindows(); }

        int init() override { return 0; }

        int mainloop() override { return 0; }

        void release() override {
        }

    private:
        Renderer & m_render;
    };
}

using namespace NSTextureLoadTest;

class TextureLoadTest : public testing::Test
{
public:
    TextureLoadTest() : m_window(nullptr)
        , m_context(nullptr)
    {
        m_window = new TestApplicationWindow(m_render);
        m_assetFactory = new FileAssetFactory(ASSET_ROOT);
    }

    ~TextureLoadTest()
    {
        delete m_assetFactory;
        delete m_window;
    }

    void SetUp() override { m_context = new Context(m_render, m_assetFactory); }

    void TearDown() override
    {
        delete m_context;
        m_context = nullptr;
    }

protected:
    Ref<Demo> m_demo;
    TestApplicationWindow* m_window;
    Context* m_context;

    Renderer m_render;
    IAssetFactory* m_assetFactory;
};

//
#define TEXTURE_2D_UUID "54484011-5c9e-4479-81f4-937888131e60"
#define TEXTURE_NAME "normap.jpg"
#define TEXTURE_PATH TEXTURE_ROOT TEXTURE_NAME 

TEST_F(TextureLoadTest, Load2DTexture)
{
    // given
    Texture2DResRef resource = m_context->Load<Texture2DRes>(new TextureFromBitmap("texture", TEXTURE_PATH, TEXTURE_2D_UUID));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_context->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ASSERT_STREQ(TEXTURE_2D_UUID, resource->GetUuid().c_str());

    ASSERT_NE(nullptr, (*resource)->GetTexture2D());
    ASSERT_NE(nullptr, (*resource)->GetTextureResource());
    ASSERT_NE(nullptr, (*resource)->GetShaderResourceView());
}

namespace
{
    const char *cubemapNames[] = {
        "yoko_posx.jpg",
        "yoko_negx.jpg",
        "yoko_posy.jpg",
        "yoko_negy.jpg",
        "yoko_posz.jpg",
        "yoko_negz.jpg",
    };
}

#define TEXTURE_CUBE_UUID "af6404fa-a4f7-4b01-8c8b-1218faf6d35c"
//
TEST_F(TextureLoadTest, LoadCubeMap)
{
    // given
    std::vector<std::string> filenames;
    std::transform(cubemapNames, cubemapNames + 6, std::back_inserter(filenames), [](std::string s)->std::string {return std::string(TEXTURE_ROOT) + s; });

    TextureCubeResRef resource = m_context->Load<TextureCubeRes>(new TextureCubemapFromBitmap("texture", filenames, TEXTURE_CUBE_UUID));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_context->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ASSERT_STREQ(TEXTURE_CUBE_UUID, resource->GetUuid().c_str());

    ASSERT_NE(nullptr, (*resource)->GetTexture2D());
    ASSERT_NE(nullptr, (*resource)->GetTextureResource());
    ASSERT_NE(nullptr, (*resource)->GetShaderResourceView());
}

#define TEXTURE_NOISE_UUID "91bde168-b40e-4c81-b974-2977014e5460"

//
TEST_F(TextureLoadTest, LoadNoiseMap)
{
    // given
    Texture2DResRef resource = m_context->Load<Texture2DRes>(new TextureNoiseMap("noiseMap", 256, TEXTURE_NOISE_UUID));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_context->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ASSERT_STREQ(TEXTURE_NOISE_UUID, resource->GetUuid().c_str());

    ASSERT_NE(nullptr, (*resource)->GetTexture2D());
    ASSERT_NE(nullptr, (*resource)->GetTextureResource());
    ASSERT_NE(nullptr, (*resource)->GetShaderResourceView());
}
