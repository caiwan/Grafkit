/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include "render/camera.h"

#include "utils/persistence/dynamics.h"
#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"

using namespace Grafkit;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;
/*
    Tests if every used animation key types could've been persisted
*/

TEST(Persistent_Camera, CameraPersistenceTest)
{
    TestArchiver ar(256, true);

    // given
    CameraRef camera = new Camera();

    // when 

    camera->Store(ar);

    // then 

    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    // 

    FAIL() << "Not implemented";

    // + Valid
}
