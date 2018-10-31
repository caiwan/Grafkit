#include "stdafx.h"

#include <gtest/gtest.h>

#include "scene/scene.h"

#include "application.h"
#include "context.h"
#include "demo.h"

#include "core/system.h"

#include "render/material.h"
#include "render/model.h"
#include "render/mesh.h"

#include "core/Music.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define JSON_PATH "test_mesh.json"

class LoadMeshTest : public testing::Test
{
public:
    LoadMeshTest() {
    }

    virtual ~LoadMeshTest() {
    }

    void SetUp() override { m_app = std::make_unique<Testing::TestApplicationContext>(); }

    void TearDown() override {
        m_app.release();
    }

protected:
    std::unique_ptr<Testing::TestApplicationContext> m_app;
};


// ============================================================================================

namespace Uuids
{
    constexpr char* torusMeshUuid = "af6404fa-a4f7-4b01-8c8b-1218faf6d35c";
    constexpr char* torusModelUuid = "ce54d354-f867-4ff4-a802-070858691c80";
}

TEST_F(LoadMeshTest, MeshLoad)
{
    // given: context
    m_app->GetContext().LoadDemo(JSON_PATH);
    m_app->GetContext().DoPrecalc();

    // when
    MeshRef mesh = m_app->SafeGetObject<Mesh>(Uuids::torusMeshUuid);

    // then
    ASSERT_TRUE(mesh);
    ASSERT_STREQ(Uuids::torusMeshUuid, mesh->GetUuid().c_str());
    ASSERT_STREQ("torus.obj", mesh->GetName().c_str());

    // ... 
}
#if 0

namespace Uuids {
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

#endif
