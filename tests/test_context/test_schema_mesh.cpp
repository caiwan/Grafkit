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
#include "render/mesh.h"

#include "core/Music.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define JSON_PATH "schema_mesh.json"
namespace NSSchemaMeshTest
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

using namespace NSSchemaMeshTest;

class SchemaMeshTest : public testing::Test
{
public:
    SchemaMeshTest() : m_window(nullptr)
        , m_context(nullptr) {
        m_window = new TestApplicationWindow(m_render);
        m_assetFactory = new FileAssetFactory("tests/assets/");
    }

    ~SchemaMeshTest()
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

namespace Uuids{
    const char * torusMeshUuid = "af6404fa-a4f7-4b01-8c8b-1218faf6d35c";
}

TEST_F(SchemaMeshTest, MeshLoad)
{
    // given: context
    this->BuildDemo();

    // when
    ASSERT_TRUE(m_demo.Valid());
    MeshRef mesh = SafeGetObject<Mesh>(Uuids::torusMeshUuid);
    
    // then
    ASSERT_TRUE(mesh);
    ASSERT_STREQ(Uuids::torusMeshUuid, mesh->GetUuid().c_str());
    ASSERT_STREQ("torus.obj", mesh->GetName().c_str());

    // ... 
}

namespace Uuids {
    const char * torusModelUuid = "ce54d354-f867-4ff4-a802-070858691c80";
}

TEST_F(SchemaMeshTest, ModelLoad)
{
    // given: context
    this->BuildDemo();

    // when
    ASSERT_TRUE(m_demo.Valid());
    ModelRef model = SafeGetObject<Model>(Uuids::torusModelUuid);

    // then
    ASSERT_TRUE(model);
    ASSERT_STREQ(Uuids::torusModelUuid, model->GetUuid().c_str());
    ASSERT_STREQ("Torus", model->GetName().c_str());

    // 
    MeshRef  mesh = model->GetMesh();

    ASSERT_TRUE(mesh);
    ASSERT_STREQ(Uuids::torusMeshUuid, mesh->GetUuid().c_str());
    ASSERT_STREQ("torus.obj", mesh->GetName().c_str());

    // 
    ASSERT_TRUE(model->GetMaterial());
}