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

TEST(Persistent_Scenegraph, DISABLED_ScenegraphPersistenceTestTest)
{
    TestArchiver ar(256 * 4096, true);

    // given

    // -- actors
    ActorRef cameraActor = new Actor();
    cameraActor->SetName("CameraActor");

    cameraActor->Matrix().Identity();
    cameraActor->Matrix().Translate(0, 0, 10);

    ActorRef cubeActor = new Actor();
    cubeActor->SetName("CubeActor");

    ActorRef rootActor = new Actor();
    rootActor->SetName("RootActor");

    rootActor->AddChild(cameraActor);
    rootActor->AddChild(cubeActor);

    // -- scenegraph
    SceneGraphRef scenegraph = new SceneGraph();
    scenegraph->SetName("Scene");
    scenegraph->SetRootNode(rootActor);
    
    // when
    scenegraph->Store(ar);

    // then
    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    // -- 
    SceneGraphRef loadedScenegraph = dynamic_cast<SceneGraph*>(Persistent::Load(ar));
    ASSERT_TRUE(loadedScenegraph.Valid());
    EXPECT_STREQ("Scene", loadedScenegraph->GetName().c_str());
    EXPECT_STREQ(scenegraph->GetUuid().c_str(), loadedScenegraph->GetUuid().c_str());

    // -- 
    ActorRef loadedRootActor = loadedScenegraph->GetRootNode();
    ASSERT_TRUE(loadedRootActor.Valid());
    EXPECT_STREQ("RootActor", loadedRootActor->GetName().c_str());
    EXPECT_STREQ(rootActor->GetUuid().c_str(), loadedRootActor->GetUuid().c_str());

    EXPECT_EQ(2, loadedRootActor->GetChildrenCount());

    // --- 
    ActorRef loadedCameraActor = loadedRootActor->GetChild(0);
    EXPECT_STREQ("CameraActor", loadedCameraActor->GetName().c_str());
    EXPECT_STREQ(cameraActor->GetUuid().c_str(), loadedCameraActor->GetUuid().c_str());

    // --- 
    ActorRef loadedCubeActor = loadedRootActor->GetChild(1);
    EXPECT_STREQ("CubeActor", loadedCubeActor->GetName().c_str());
    EXPECT_STREQ(cubeActor->GetUuid().c_str(), loadedCubeActor->GetUuid().c_str());

}
