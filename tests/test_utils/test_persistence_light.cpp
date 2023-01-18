/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include "render/Light.h"

#include "utils/persistence/dynamics.h"
#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"

using namespace Grafkit;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;

TEST(Persistent_Light, DISABLED_LightPersistenceTest)
{
    TestArchiver ar(256, true);

    // given
    //LightRef light = new Light();

    FAIL() << "Not implemented";

    // when

    //then
    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

}
