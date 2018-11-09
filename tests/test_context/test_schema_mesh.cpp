#include "stdafx.h"

#include <gtest/gtest.h>

#include "scene/scene.h"

#include "application.h"
#include "context.h"

#include "render/material.h"
#include "render/model.h"
#include "render/mesh.h"

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

    void SetUp() override
    {
        m_app = std::make_unique<Testing::TestApplicationContext>();
        m_app->Initialize();
    }

    void TearDown() override { m_app->Release(); }

protected:
    std::unique_ptr<Testing::TestApplicationContext> m_app;
};


// ============================================================================================

namespace Uuids
{
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
