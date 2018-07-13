/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include "animation/animation.h"

#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"
#include "render/model.h"
#include "render/Material.h"

#include "builtin_data/cube.h"
using namespace Grafkit;
using namespace GrafkitData;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;
/*
    Tests if every used animation key types could've been persisted
*/

TEST(Persistent_Model, ModelPersistenceTest)
{
    TestArchiver ar(12*4096, true);

    // given
    ModelRef model = new Model(CreateCube());
    model->SetName("Model");
    model->SetMaterial(new Material());
    model->GetMaterial()->SetName("Material");

    //when
    model->Store(ar);

    //then
    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    ModelRef loadedModel = dynamic_cast<Model*>(Persistent::Load(ar));

    ASSERT_TRUE(loadedModel.Valid());
    ASSERT_TRUE(loadedModel->GetMaterial().Valid());
    ASSERT_TRUE(loadedModel->GetMesh().Valid());

    EXPECT_EQ("Model", loadedModel->GetName().c_str());
    EXPECT_EQ(model->GetUuid().c_str(), loadedModel->GetName().c_str());

    EXPECT_EQ("Material", loadedModel->GetMaterial()->GetName());
    EXPECT_EQ(model->GetMaterial()->GetUuid().c_str(), loadedModel->GetMaterial()->GetUuid().c_str());
    
}

// + multitrack
