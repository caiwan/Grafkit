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

#include "builtin_data/cube.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;
using namespace GrafkitData;

#define ASSET_ROOT "tests/assets/"
#define MESH_ROOT "models/"
#define MESH_PERSIST MESH_ROOT "testMesh.mesh"
#define MESH_SRC MESH_ROOT "src/torus.obj"
#define MESH_PATH ASSET_ROOT MESH_PERSIST

#define MESH_UUID "b25eafaa-399e-4c87-b254-04b0de2ec436"
#define MESH_PERSIST_UUID "4ad71d86-0e40-494e-9376-36bffd21b2e7"

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
        // generate cbe 
        m_mesh = CreateCube();
        m_mesh->SetName("Cube");
        m_mesh->SetUuid(MESH_PERSIST_UUID);

        FILE* fp;
        errno_t err = fopen_s(&fp, MESH_PATH, "wb");
        assert(0 == err);

        ArchiveFile archive(fp, true);
        m_mesh->Store(archive);
        fclose(fp);

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
    Ref<Resource<Mesh>> resource = m_context->Load<Resource<Mesh>>(new MeshOBJLoader("testMesh", MESH_SRC, "", MESH_UUID));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_context->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ASSERT_STREQ(MESH_UUID, resource->GetUuid().c_str());
    ASSERT_STREQ(MESH_UUID, (*resource)->GetUuid().c_str());

    ASSERT_NE(nullptr, (*resource)->GetPointer("POSITION"));
    ASSERT_NE(nullptr, (*resource)->GetPointer("NORMAL"));
    ASSERT_NE(nullptr, (*resource)->GetPointer("TEXCOORD0"));
}

//
TEST_F(MeshLoadTest, LoadPersisted)
{
    // given
    SaveTestData();
    Ref<Resource<Mesh>> resource = m_context->Load<Resource<Mesh>>(new MeshOBJLoader("testMesh", "", MESH_PERSIST, MESH_PERSIST_UUID));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_context->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ASSERT_STREQ(MESH_PERSIST_UUID, resource->GetUuid().c_str());
    ASSERT_STREQ(MESH_PERSIST_UUID, (*resource)->GetUuid().c_str());

    ASSERT_NE(nullptr, (*resource)->GetPointer("POSITION"));
    ASSERT_NE(nullptr, (*resource)->GetPointer("NORMAL"));
}

