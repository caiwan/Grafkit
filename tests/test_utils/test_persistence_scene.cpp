/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include "render/model.h"
#include "render/Material.h"
#include "render/Camera.h"
#include "render/SceneGraph.h"

#include "animation/animation.h"

#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"

#include "builtin_data/cube.h"

using namespace Grafkit;
using namespace GrafkitData;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;
/*
    Tests if every used animation key types could've been persisted
*/


TEST(Persistent_Scene, ScenePersistenceTest)
{
    TestArchiver ar(256 * 4096, true);

    // given
    // -- model 
    ModelRef model = new Model(CreateCube());
    model->SetName("Cube");
    model->GetMaterial()->SetName("GridMaterial");

    // -- camera
    CameraRef camera = new Camera();
    camera->SetName("Camera");

    ActorRef cameraActor = new Actor(camera);
    cameraActor->SetName("CameraActor");

    cameraActor->Matrix().Identity();
    cameraActor->Matrix().Translate(0, 0, 10);

    // -- actors
    ActorRef cubeActor = new Actor(model);
    cubeActor->SetName("CubeActor");

    ActorRef rootActor = new Actor();
    rootActor->SetName("RootActor");

    rootActor->AddChild(cameraActor);
    rootActor->AddChild(cubeActor);

    // -- scenegraph
    SceneGraphRef scenegraph = new SceneGraph();
    scenegraph->SetName("Scene");
    scenegraph->SetRootNode(rootActor);

    // -- animations
    // -- automations

    // when
    scenegraph->Store(ar);

    // then
    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    // -- 
    SceneGraphRef loadedScenegraph = dynamic_cast<SceneGraph*>(Persistent::Load(ar));
    ASSERT_TRUE(loadedScenegraph.Valid());
    EXPECT_STREQ("Scene", loadedScenegraph->GetName().c_str()); // + test All the uuids
    EXPECT_STREQ(scene->, loadedScenegraph->GetName().c_str());

    // -- 
    ActorRef loadedRootActor = loadedScenegraph->GetRootNode();
    ASSERT_TRUE(loadedRootActor.Valid());
    EXPECT_STREQ("RootActor", loadedRootActor->GetName().c_str());

    EXPECT_EQ(2, loadedRootActor->GetChildrenCount());

    // --- 
    ActorRef loadedCameraActor = loadedRootActor->GetChild(0);
    EXPECT_STREQ("CameraActor", loadedCameraActor->GetName().c_str());

    EXPECT_EQ(1, loadedCameraActor->GetEntityCount());
    EXPECT_TRUE(loadedCameraActor->GetEntity(0).Valid());

    // --- 
    ActorRef loadedCubeActor = loadedRootActor->GetChild(1);
    EXPECT_STREQ("CubeActor", loadedCubeActor->GetName().c_str());

    EXPECT_TRUE(1, loadedCubeActor->GetEntityCount());
    EXPECT_TRUE(loadedCubeActor->GetEntity(0).Valid());

}

// + multitrack
