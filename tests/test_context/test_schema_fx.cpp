#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/asset/AssetFile.h"

#include "scene/scene.h"

#include "demo.h"
#include "context.h"
#include "schema.h"

#include "core/system.h"

#include "render/shader.h"

#include "core/Music.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define JSON_PATH "schema_fx.json"

#if 0

namespace NSMaterialTest
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
        Renderer& m_render;
    };
}

using namespace NSMaterialTest;

class ContextShaderTest : public testing::Test
{
public:
    ContextShaderTest() : m_window(nullptr)
        , m_context(nullptr)
    {
        m_window = new TestApplicationWindow(m_render);
        m_assetFactory = new FileAssetFactory("tests/assets/");
    }

    ~ContextShaderTest()
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
    Context* m_context;

    Renderer m_render;
    IAssetFactory* m_assetFactory;
};

// ============================================================================================

namespace Uuids
{
    const char* vertexShaderUuid = "43ae2c7a-b8e7-4eb0-a888-7ce1c2fa90a1";
    const char* pixelShaderUuid = "5f72e9aa-93f5-4d86-ada8-9543b625337e";
    const char* pbrShaderUuid = "a1c0dd7b-6956-45be-9821-4c7a56819d5b";
    const char* ssaoShaderUuid = "befdc2c1-9ecc-4e0f-95f4-eb9bc38d0130";
    const char* ssaoBlurShaderUuid = "996176c2-dade-4741-b596-fad888af3768";
    const char* bloomShaderUuid = "dc02f0c2-b4c8-4710-ba52-f9b441f4b341";
    const char* chromaticAbberationShaderUuid = "082484f0-7237-4a24-b6c2-313fd4456ae9";
    const char* dofCalcShaderUuid = "7c49edd4-9fab-4faf-916f-319feb0b9a17";
    const char* dofBlurShaderUuid = "5188f87c-c66b-4207-af8d-835d9bea3df7";
    const char* dofJoinShaderUuid = "d6ed270e-b5d1-4575-ae9a-94168f4f094a";
    const char* fxaaShaderUuid = "e3351e5d-af09-4f64-bbea-423a49f74432";
}

TEST_F(ContextShaderTest, ShaderLoad)
{
    // given: context
    // when
    this->BuildDemo();
    ASSERT_TRUE(m_demo.Valid());

    // then

    ShaderResRef vertexShader = SafeGetResource<Shader>(Uuids::vertexShaderUuid);
    ShaderResRef pixelShader = SafeGetResource<Shader>(Uuids::pixelShaderUuid);
    //ShaderResRef pbrShader = SafeGetResource<Shader>(Uuids::pbrShaderUuid);
    //ShaderResRef ssaoShader = SafeGetResource<Shader>(Uuids::ssaoShaderUuid);
    //ShaderResRef ssaoBlurShader = SafeGetResource<Shader>(Uuids::ssaoBlurShaderUuid);
    //ShaderResRef bloomShader = SafeGetResource<Shader>(Uuids::bloomShaderUuid);
    //ShaderResRef ChromaticAbberationShader = SafeGetResource<Shader>(Uuids::chromaticAbberationShaderUuid);
    //ShaderResRef dofCalcShader = SafeGetResource<Shader>(Uuids::dofCalcShaderUuid);
    //ShaderResRef dofBlurShader = SafeGetResource<Shader>(Uuids::dofBlurShaderUuid);
    //ShaderResRef dofJoinShader = SafeGetResource<Shader>(Uuids::dofJoinShaderUuid);
    ShaderResRef fxaaShader = SafeGetResource<Shader>(Uuids::fxaaShaderUuid);

    ASSERT_TRUE(vertexShader);
    ASSERT_TRUE(pixelShader);
    //ASSERT_TRUE(pbrShader);
    //ASSERT_TRUE(ssaoShader);
    //ASSERT_TRUE(ssaoBlurShader);
    //ASSERT_TRUE(bloomShader);
    //ASSERT_TRUE(ChromaticAbberationShader);
    //ASSERT_TRUE(dofCalcShader);
    //ASSERT_TRUE(dofBlurShader);
    //ASSERT_TRUE(dofJoinShader);
    ASSERT_TRUE(fxaaShader);

    ASSERT_TRUE(*vertexShader);
    ASSERT_TRUE(*pixelShader);
    //ASSERT_TRUE(*pbrShader);
    //ASSERT_TRUE(*ssaoShader);
    //ASSERT_TRUE(*ssaoBlurShader);
    //ASSERT_TRUE(*bloomShader);
    //ASSERT_TRUE(*ChromaticAbberationShader);
    //ASSERT_TRUE(*dofCalcShader);
    //ASSERT_TRUE(*dofBlurShader);
    //ASSERT_TRUE(*dofJoinShader);
    ASSERT_TRUE(*fxaaShader);

    ASSERT_STREQ(Uuids::vertexShaderUuid, vertexShader->GetUuid().c_str());
    ASSERT_STREQ(Uuids::pixelShaderUuid, pixelShader->GetUuid().c_str());
    //ASSERT_STREQ(Uuids::pbrShaderUuid, pbrShader->GetUuid().c_str());
    //ASSERT_STREQ(Uuids::ssaoShaderUuid, ssaoShader->GetUuid().c_str());
    //ASSERT_STREQ(Uuids::ssaoBlurShaderUuid, ssaoBlurShader->GetUuid().c_str());
    //ASSERT_STREQ(Uuids::bloomShaderUuid, bloomShader->GetUuid().c_str());
    //ASSERT_STREQ(Uuids::chromaticAbberationShaderUuid, ChromaticAbberationShader->GetUuid().c_str());
    //ASSERT_STREQ(Uuids::dofCalcShaderUuid, dofCalcShader->GetUuid().c_str());
    //ASSERT_STREQ(Uuids::dofBlurShaderUuid, dofBlurShader->GetUuid().c_str());
    //ASSERT_STREQ(Uuids::dofJoinShaderUuid, dofJoinShader->GetUuid().c_str());
    ASSERT_STREQ(Uuids::fxaaShaderUuid, fxaaShader->GetUuid().c_str());

    ASSERT_STREQ("vertexShader", vertexShader->GetName().c_str());
    ASSERT_STREQ("pixelShader", pixelShader->GetName().c_str());
    //ASSERT_STREQ("pbrShader", pbrShader->GetName().c_str());
    //ASSERT_STREQ("ssaoShader", ssaoShader->GetName().c_str());
    //ASSERT_STREQ("ssaoBlurShader", ssaoBlurShader->GetName().c_str());
    //ASSERT_STREQ("bloomShader", bloomShader->GetName().c_str());
    //ASSERT_STREQ("ChromaticAbberationShader", ChromaticAbberationShader->GetName().c_str());
    //ASSERT_STREQ("dofCalcShader", dofCalcShader->GetName().c_str());
    //ASSERT_STREQ("dofBlurShader", dofBlurShader->GetName().c_str());
    //ASSERT_STREQ("dofJoinShader", dofJoinShader->GetName().c_str());
    ASSERT_STREQ("fxaaShader", fxaaShader->GetName().c_str());
}


TEST_F(ContextShaderTest, ShaderBind)
{
    // given: context
    this->BuildDemo();
    // when
    ASSERT_TRUE(m_demo.Valid());

    // then 

    ShaderResRef vertexShader = SafeGetResource<Shader>(Uuids::vertexShaderUuid);
    ShaderResRef pixelShader = SafeGetResource<Shader>(Uuids::pixelShaderUuid);
    //ShaderResRef pbrShader = SafeGetResource<Shader>(Uuids::pbrShaderUuid);
    //ShaderResRef ssaoShader = SafeGetResource<Shader>(Uuids::ssaoShaderUuid);
    //ShaderResRef ssaoBlurShader = SafeGetResource<Shader>(Uuids::ssaoBlurShaderUuid);
    //ShaderResRef bloomShader = SafeGetResource<Shader>(Uuids::bloomShaderUuid);
    //ShaderResRef ChromaticAbberationShader = SafeGetResource<Shader>(Uuids::chromaticAbberationShaderUuid);
    //ShaderResRef dofCalcShader = SafeGetResource<Shader>(Uuids::dofCalcShaderUuid);
    //ShaderResRef dofBlurShader = SafeGetResource<Shader>(Uuids::dofBlurShaderUuid);
    //ShaderResRef dofJoinShader = SafeGetResource<Shader>(Uuids::dofJoinShaderUuid);
    ShaderResRef fxaaShader = SafeGetResource<Shader>(Uuids::fxaaShaderUuid);

    ASSERT_TRUE(vertexShader);
    ASSERT_TRUE(pixelShader);
    //ASSERT_TRUE(pbrShader);
    //ASSERT_TRUE(ssaoShader);
    //ASSERT_TRUE(ssaoBlurShader);
    //ASSERT_TRUE(bloomShader);
    //ASSERT_TRUE(ChromaticAbberationShader);
    //ASSERT_TRUE(dofCalcShader);
    //ASSERT_TRUE(dofBlurShader);
    //ASSERT_TRUE(dofJoinShader);
    ASSERT_TRUE(fxaaShader);

    ASSERT_EQ(m_demo->GetVs()->Get(), vertexShader->Get());
    ASSERT_EQ(m_demo->GetPs()->Get(), pixelShader->Get());
    //ASSERT_EQ(m_demo->GetFxPbr()->Get(), pbrShader->Get());
    //ASSERT_EQ(m_demo->GetFxSsao()->Get(), ssaoShader->Get());
    //ASSERT_EQ(m_demo->GetFxSsaoBlur()->Get(), ssaoBlurShader->Get());
    //ASSERT_EQ(m_demo->GetFxBloom()->Get(), bloomShader->Get());
    //ASSERT_EQ(m_demo->GetFxChromaticAbberation()->Get(), ChromaticAbberationShader->Get());
    //ASSERT_EQ(m_demo->GetFxDofCompute()->Get(), dofCalcShader->Get());
    //ASSERT_EQ(m_demo->GetFxDofBlur()->Get(), dofBlurShader->Get());
    //ASSERT_EQ(m_demo->GetFxDofJoin()->Get(), dofJoinShader->Get());
    ASSERT_EQ(m_demo->GetFxaa()->Get(), fxaaShader->Get());

}

#endif