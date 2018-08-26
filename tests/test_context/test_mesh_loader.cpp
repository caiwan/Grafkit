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
#include "render/mesh.h"
#include "MeshLoader.h"
#include "utils/persistence/archive.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define ASSET_ROOT "tests/assets/"
// #define MODEL_ROOT ""
#define MESH_PERSIST "testMesh.mesh"
#define MESH_SRC "models/src/torus.obj"
#define MESH_PATH ASSET_ROOT MESH

namespace NSMeshLoadTest
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

using namespace NSMeshLoadTest;

class MeshLoadTest : public testing::Test
{
public:
    MeshLoadTest() : m_window(nullptr)
        , m_context(nullptr)
    {
        m_window = new TestApplicationWindow(m_render);
        m_assetFactory = new FileAssetFactory(ASSET_ROOT);
    }

    ~MeshLoadTest()
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

    void SaveTestData()
    {
        m_mesh = new Mesh();

    }

protected:
    Ref<Demo> m_demo;
    TestApplicationWindow* m_window;
    Context* m_context;

    Renderer m_render;
    IAssetFactory* m_assetFactory;

    MeshRef m_mesh;
};

//
TEST_F(MeshLoadTest, LoadOBJViaAssimp)
{
    // given
    SaveTestData();
    Ref<Resource<Mesh>> resource = m_context->Load<Resource<Mesh>>(new MeshOBJLoader("testMesh", MESH_SRC, ""));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_context->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    //(*resource)->GetPointer("");
}
