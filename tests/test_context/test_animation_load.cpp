#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/AssetFile.h"

#include "experimental/scene.h"

#include "demo.h"
#include "context.h"
#include "schema.h"

#include "core/system.h"

#include "render/material.h"
#include "render/model.h"

#include "core/Music.h"
#include "animation/actoranimation.h"
#include "experimental/AnimationLoader.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define JSON_PATH "schema.json"

namespace NSAnimationLoadTest
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

using namespace NSAnimationLoadTest;

class AnimationLoadTest : public testing::Test
{
public:
    AnimationLoadTest() : m_window(nullptr)
        , m_context(nullptr)
    {
        m_window = new TestApplicationWindow(m_render);
        m_assetFactory = new FileAssetFactory("tests/assets/");
    }

    ~AnimationLoadTest()
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
        try
        {
            SchemaBuilder builder;
            IAssetFactory* af = m_context->GetAssetFactory();
            IAssetRef file = af->Get(JSON_PATH);
            builder.LoadFromAsset(file, dynamic_cast<IResourceManager*>(m_context));
            m_demo = builder.GetDemo();
        }
        catch (std::exception& e)
        {
            FAIL() << e.what();
        }
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

//

TEST_F(AnimationLoadTest, ActorAnimation)
{
    // given
    Ref<Resource<ActorAnimation>> resource = m_context->Load<Resource<ActorAnimation>>(new ActorAnimationLoader("testActorAnimation"));

    // when
    m_context->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);
}
