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

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define JSON_PATH "schema.json"
namespace NSContextPregnancyTest
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

using namespace NSContextPregnancyTest;

class ContextPregnancyTest : public testing::Test
{
public:
    ContextPregnancyTest() : m_window(nullptr)
        , m_context(nullptr) {
        m_window = new TestApplicationWindow(m_render);
        m_assetFactory = new FileAssetFactory("tests/assets/");
    }

    ~ContextPregnancyTest()
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

TEST_F(ContextPregnancyTest, JsonLoad)
{
    // given: context
    this->BuildDemo();
    ASSERT_TRUE(m_demo.Valid());

    // when
    

    // then
    ASSERT_EQ(1, m_demo->GetSceneCount());
    ASSERT_TRUE(m_demo->GetScene(0));
    ASSERT_TRUE(m_demo->GetMusic());

    ASSERT_TRUE(m_demo->GetPs());
    ASSERT_TRUE(m_demo->GetPs()->Valid());

    ASSERT_TRUE(m_demo->GetVs());
    ASSERT_TRUE(m_demo->GetVs()->Valid());

    ASSERT_EQ(1, m_demo->GetAnimationCount());
    ASSERT_TRUE(m_demo->GetAnimation(0));
}
TEST_F(ContextPregnancyTest, SceneActorTest)
{
    // given: context
    this->BuildDemo();


    // when
    SceneRef scene = m_demo->GetScene(0)->Get();
    ASSERT_TRUE(scene);

    SceneGraphRef scenegraph = scene->GetSceneGraph();
    ASSERT_TRUE(scenegraph);

    // then

    ASSERT_TRUE(scenegraph->GetRootNode());

    size_t actorCount = scenegraph->GetNodeCount();
    ASSERT_EQ(6, actorCount);

    for (size_t i = 0; i < actorCount; i++) { ASSERT_TRUE(scenegraph->GetNode(i)) << "Node " << i; }

}

TEST_F(ContextPregnancyTest, DISABLED_SceneEntityTest)
{
    // given: context
    this->BuildDemo();

    // when
    SceneRef scene = m_demo->GetScene(0)->Get();
    ASSERT_TRUE(scene);

    SceneGraphRef scenegraph = scene->GetSceneGraph();
    ASSERT_TRUE(scenegraph);

    // then
}

TEST_F(ContextPregnancyTest, SceneLightTest)
{
    // given: context
    this->BuildDemo();

    // when
    SceneRef scene = m_demo->GetScene(0)->Get();
    ASSERT_TRUE(scene);
    // then

    size_t actorCount = scene->GetLightCount();
    ASSERT_EQ(1, actorCount);

    for (size_t i = 0; i < actorCount; i++) { ASSERT_TRUE(scene->GetLight(i)) << "Light Node " << i; }
}

TEST_F(ContextPregnancyTest, SceneCameraTest)
{
    // given: context
    this->BuildDemo();

    // when
    SceneRef scene = m_demo->GetScene(0)->Get();
    ASSERT_TRUE(scene);
    // then

    CameraRef camera1 = scene->GetCamera("Camera01");
    ASSERT_TRUE(camera1);
    //ASERT_STREQ("", camera1->GetUuid().c_str());

    CameraRef camera2 = scene->GetCamera("Camera02");
    ASSERT_TRUE(camera2);
    //ASERT_STREQ("", camera2->GetUuid().c_str());

    size_t actorCount = scene->GetCameraCount();
    ASSERT_EQ(2, actorCount);

    for (size_t i = 0; i < actorCount; i++) { ASSERT_TRUE(scene->GetCamera(i)) << "Camera Node " << i; }

}
