/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include "render/SceneGraph.h"
#include "render/Mesh.h"
#include "render/Model.h"
#include "render/Material.h"
#include "animation/animation.h"

#include "utils/persistence/dynamics.h"
#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"

using namespace Grafkit;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;

#define DEFINE_PREGNANCY_TEST(CLASS) \
	TEST(Persistence_Pregnancy, ##CLASS##_PersistSmokeTest) {\
		TestArchiver archive(16 * 4096, true);\
\
		Ref<CLASS> object = new CLASS();\
		PERSIST_REFOBJECT(archive, object);\
\
		archive.ResetCrsr();\
		archive.SetDirection(false);\
\
		Ref<CLASS> object_read;\
		PERSIST_REFOBJECT(archive, object_read);\
\
		ASSERT_TRUE(object_read.Valid());\
		ASSERT_TRUE(object.Get() != object_read.Get());\
	}

DEFINE_PREGNANCY_TEST(SceneGraph)
DEFINE_PREGNANCY_TEST(Actor)
DEFINE_PREGNANCY_TEST(Material)
DEFINE_PREGNANCY_TEST(Mesh)
DEFINE_PREGNANCY_TEST(Model)
