#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/asset/AssetFile.h"

#include "scene/scene.h"

#include "demo.h"
#include "context.h"
#include "schema.h"

#include "core/system.h"

#include "render/material.h"
#include "render/model.h"
#include "render/texture.h"

#include "core/Music.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#if 0

#define JSON_PATH "schema_texture.json"
namespace NSMaterialTest
{
    class TestApplicationWindow : public System
    {
    public:
        explicit TestApplicationWindow(Renderer &render) : m_render(render)
        {
            InitializeWindows(320, 240);
            m_render.Initialize(m_window.getRealWidth(), m_window.getRealHeight(), true, this->m_window.getHWnd(), false);
        }

        ~TestApplicationWindow()
        {
            ShutdownWindows();
        }

        int init() override { return 0; }

        int mainloop() override { return 0; }

        void release() override { }

    private:
        Renderer & m_render;
    };
}

using namespace NSMaterialTest;

class ContextMaterialTest : public testing::Test
{
public:
    ContextMaterialTest() : m_window(nullptr)
        , m_context(nullptr) {
        m_window = new TestApplicationWindow(m_render);
        m_assetFactory = new FileAssetFactory("tests/assets/");
    }

    ~ContextMaterialTest()
    {
        delete m_assetFactory;
        delete m_window;
    }

    void SetUp() override { m_context = new Context(m_render, m_assetFactory); }

    void TearDown() override {
        delete m_context;
        m_context = nullptr;
    }

    void BuildDemo()
    {
        //try
        //{
        SchemaBuilder builder;
        IAssetFactory* af = m_context->GetAssetFactory();
        IAssetRef file = af->Get(JSON_PATH);
        builder.LoadFromAsset(file, dynamic_cast<IResourceManager*>(m_context));

        m_demo = builder.GetDemo();

        m_context->DoPrecalc();
        builder.Initialize(m_context);

        //m_context->Intitialize();
        m_demo->Initialize(m_render);
        /*}
        catch (std::exception& e)
        {
            FAIL() << e.what();
        }*/
    }

    template <class T>
    Ref<T> SafeGetObject(const char* uuid)
    {
        assert(uuid);
        IResourceManager* resman = dynamic_cast<IResourceManager*>(m_context);
        assert(resman);
        Ref<Resource<T>> tResource = resman->GetByUuid<Resource<T>>(uuid);
        if (!tResource)
            return nullptr;
        if (!*tResource)
            return nullptr;
        return tResource->Get();
    }

    template <class T>
    Ref<Resource<T>> SafeGetResource(const char* uuid)
    {
        assert(uuid);
        Ref<Resource<T>> tResource = m_context->GetByUuid<Resource<T>>(uuid);
        if (!tResource)
            return nullptr;
        return tResource;
    }

protected:
    Ref<Demo> m_demo;
    TestApplicationWindow* m_window;
    Context * m_context;

    Renderer m_render;
    IAssetFactory* m_assetFactory;

};

// ============================================================================================

namespace Uuids
{
    const char * normapUuid = "4c8c7210-49b5-41a6-9681-fd359cb4d6ab";
    const char * checkerBoardUuid = "de386543-5cd8-4d87-bd16-9b04e037a393";
    const char * skyTextureUuid = "2fa2effd-93fa-4984-9647-4b31127a3cbe";
    const char * skyRadianceTextureUuid = "ed9cd55c-34c9-4644-acfa-35bfb2fbb31a";
    const char * noise256TextureUuid = "d519b673-764c-4530-a28f-df9ee6133eae";
    const char * noise512TextureUuid = "8a92cb3a-5b77-4d4f-9e11-3df45b269b03";
}

TEST_F(ContextMaterialTest, TextureLoad)
{
    // given: context
    this->BuildDemo();
    ASSERT_TRUE(m_demo.Valid());

    // when
    Texture2DResRef normapTexture = SafeGetResource<Texture2D>(Uuids::normapUuid);
    Texture2DResRef checkerBoardTexture = SafeGetResource<Texture2D>(Uuids::checkerBoardUuid);
    TextureCubeResRef skyTextureTexture = SafeGetResource<TextureCube>(Uuids::skyTextureUuid);
    TextureCubeResRef skyRadianceTextureTexture = SafeGetResource<TextureCube>(Uuids::skyRadianceTextureUuid);
    Texture2DResRef noise256TextureTexture = SafeGetResource<Texture2D>(Uuids::noise256TextureUuid);
    Texture2DResRef noise512TextureTexture = SafeGetResource<Texture2D>(Uuids::noise512TextureUuid);

    // then

    // bitmap
    ASSERT_TRUE(normapTexture);
    ASSERT_TRUE((*normapTexture));
    ASSERT_STREQ(Uuids::normapUuid, normapTexture->GetUuid().c_str());
    ASSERT_STREQ("normap", normapTexture->GetName().c_str());

    ASSERT_TRUE((*normapTexture)->GetTexture2D());
    ASSERT_TRUE((*normapTexture)->GetTextureResource());
    ASSERT_TRUE((*normapTexture)->GetShaderResourceView());

    // 
    ASSERT_TRUE(checkerBoardTexture);
    ASSERT_TRUE((*checkerBoardTexture));
    ASSERT_STREQ(Uuids::checkerBoardUuid, checkerBoardTexture->GetUuid().c_str());
    
    ASSERT_STREQ("UVAddressBoard", checkerBoardTexture->GetName().c_str());
    ASSERT_TRUE((*checkerBoardTexture)->GetTexture2D());
    ASSERT_TRUE((*checkerBoardTexture)->GetTextureResource());
    ASSERT_TRUE((*checkerBoardTexture)->GetShaderResourceView());

    // cubemap
    ASSERT_TRUE(skyTextureTexture);
    ASSERT_TRUE((*skyTextureTexture));
    ASSERT_STREQ(Uuids::skyTextureUuid, skyTextureTexture->GetUuid().c_str());
    ASSERT_STREQ("Sky1", skyTextureTexture->GetName().c_str());

    ASSERT_TRUE((*skyTextureTexture)->GetTexture2D());
    ASSERT_TRUE((*skyTextureTexture)->GetTextureResource());
    ASSERT_TRUE((*skyTextureTexture)->GetShaderResourceView());

    // 
    ASSERT_TRUE(skyRadianceTextureTexture);
    ASSERT_TRUE((*skyRadianceTextureTexture));
    ASSERT_STREQ(Uuids::skyRadianceTextureUuid, skyRadianceTextureTexture->GetUuid().c_str());
    ASSERT_STREQ("SkyRadiance", skyRadianceTextureTexture->GetName().c_str());

    ASSERT_TRUE((*skyRadianceTextureTexture)->GetTexture2D());
    ASSERT_TRUE((*skyRadianceTextureTexture)->GetTextureResource());
    ASSERT_TRUE((*skyRadianceTextureTexture)->GetShaderResourceView());

    // noise kernel
    ASSERT_TRUE(noise256TextureTexture);
    ASSERT_TRUE((*noise256TextureTexture));
    ASSERT_STREQ(Uuids::noise256TextureUuid, noise256TextureTexture->GetUuid().c_str());
    ASSERT_STREQ("Noise256", noise256TextureTexture->GetName().c_str());

    ASSERT_TRUE((*noise256TextureTexture)->GetTexture2D());
    ASSERT_TRUE((*noise256TextureTexture)->GetTextureResource());
    ASSERT_TRUE((*noise256TextureTexture)->GetShaderResourceView());

    //
    ASSERT_TRUE(noise512TextureTexture);
    ASSERT_TRUE((*noise512TextureTexture));
    ASSERT_STREQ(Uuids::noise512TextureUuid, noise512TextureTexture->GetUuid().c_str());
    ASSERT_STREQ("Noise512", noise512TextureTexture->GetName().c_str());

    ASSERT_TRUE((*noise512TextureTexture)->GetTexture2D());
    ASSERT_TRUE((*noise512TextureTexture)->GetTextureResource());
    ASSERT_TRUE((*noise512TextureTexture)->GetShaderResourceView());
}

namespace Uuids
{
    const char * normapMaterialUuid = "1c982eed-494f-4648-96fc-29f6e33becd9";
    const char * checkerMaterialUuid = "74e35451-384d-4ae9-b4bf-d630f4cebf63";
    const char * torusPbrMaterialUuid = "24dc43f2-4bb8-46a1-8614-ead866237cea";
}

TEST_F(ContextMaterialTest, MaterialLoad)
{
    // given: context
    this->BuildDemo();
    ASSERT_TRUE(m_demo.Valid());

    // when
    MaterialRef normapMaterial = SafeGetObject<Material>(Uuids::normapMaterialUuid);
    MaterialRef checkerBoardMaterial = SafeGetObject<Material>(Uuids::checkerMaterialUuid);

    ASSERT_TRUE(normapMaterial);
    ASSERT_TRUE(checkerBoardMaterial);

    // then
    ASSERT_STREQ(Uuids::normapMaterialUuid, normapMaterial->GetUuid().c_str());
    ASSERT_STREQ("NormapMaterial", normapMaterial->GetName().c_str());

    ASSERT_STREQ(Uuids::checkerMaterialUuid, checkerBoardMaterial->GetUuid().c_str());
    ASSERT_STREQ("CheckerBoardMaterial", checkerBoardMaterial->GetName().c_str());
}


TEST_F(ContextMaterialTest, TextureBoundMaterialLoad)
{
    // given: context
    this->BuildDemo();
    ASSERT_TRUE(m_demo.Valid());

    // when
    MaterialRef normapMaterial = SafeGetObject<Material>(Uuids::normapMaterialUuid);
    MaterialRef checkerBoardMaterial = SafeGetObject<Material>(Uuids::checkerMaterialUuid);
    MaterialRef torusPbrMaterial = SafeGetObject<Material>(Uuids::torusPbrMaterialUuid);

    ASSERT_TRUE(normapMaterial);
    ASSERT_TRUE(checkerBoardMaterial);
    ASSERT_TRUE(torusPbrMaterial);

    // then
    ASSERT_TRUE(normapMaterial->GetTexture("diffuse"));
    ASSERT_STREQ(Uuids::normapUuid, normapMaterial->GetTexture("diffuse")->GetName().c_str());
    ASSERT_STREQ("normap", normapMaterial->GetTexture("diffuse")->GetUuid().c_str());

    ASSERT_TRUE(checkerBoardMaterial->GetTexture("diffuse"));
    ASSERT_STREQ(Uuids::checkerBoardUuid, checkerBoardMaterial->GetTexture("diffuse")->GetName().c_str());
    ASSERT_STREQ("UVAddressBoard", checkerBoardMaterial->GetTexture("diffuse")->GetUuid().c_str());

    /*ASSERT_TRUE(torusPbrMaterial->GetTexture("diffuse"));
    ASSERT_STREQ(Uuids::torusPbrMaterialUuid, torusPbrMaterial->GetTexture("diffuse")->GetName().c_str());
    ASSERT_STREQ("TorusPbrMaterial", torusPbrMaterial->GetTexture("diffuse")->GetUuid().c_str());*/
}

#endif