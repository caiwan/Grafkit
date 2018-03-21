/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>
#include <cmath>

#include "Assert.h"

#include "render/SceneGraph.h"
#include "render/Model.h"
#include "animation/animation.h"

#include "utils/persistence/dynamics.h"
#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"

using namespace Grafkit;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;

#define DEFINE_PREGNANCY_TEST(CLASS) \
	TEST(Persistence_Pregnancy, given_##CLASS##_when_Persist_then_Load) {\
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

/*
	Tests if every used animation key types could've been persisted
*/

#define D_TEST_FLOAT(f) (f)

#define D_TEST_KEY(in, key, value, type)\
	in.m_key = key; in.m_type = type; in.m_value = value

#define ASSERT_KEY_EQ(val1, val2)\
	ASSERT_EQ(val1.m_type, val2.m_type); \
	ASSERT_FLOAT_EQ(val1.m_key, val2.m_key); \

TEST(Persistent_Animation, given_AnimationKey_when_persist_then_Load) {
	TestArchiver ar(256, true);

	// given
	Animation::Key key; D_TEST_KEY(key, D_TEST_FLOAT(.1), D_TEST_FLOAT(.1), Animation::KI_hermite);
	
	//when
	PERSIST_FIELD(ar, key);
	
	//then
	ar.ResetCrsr();
	ar.SetDirection(false);

	Animation::Key test_key;
	
	PERSIST_FIELD(ar, test_key);

	ASSERT_KEY_EQ(key, test_key);
	ASSERT_FLOAT_EQ(key.m_value, test_key.m_value);

}

/*
	Tests if every used animation track types could've been persisted
*/

TEST(Persistent_Animation, given_AnimationTrack_when_persist_then_Load) {
	TestArchiver ar(16*4096, true);

	// given
	Animation::Track track;

	for (int i = 0; i < 256; ++i) {
		float f = 1. / i;
		Animation::Key key; D_TEST_KEY(key, D_TEST_FLOAT(f), D_TEST_FLOAT(f), Animation::KI_hermite);

		track.AddKey(key);
	}
	
	//when
	track.serialize(ar);

	//then
	ar.ResetCrsr();
	ar.SetDirection(false);

	Animation::Track test_track;
	test_track.serialize(ar);

	ASSERT_EQ(track.GetKeyCount(), test_track.GetKeyCount());

	for (int i = 0; i < 256; ++i) {
		Animation::Key key = track.GetKey(i);
		Animation::Key test_key = test_track.GetKey(i);

		ASSERT_KEY_EQ(key, test_key);
		ASSERT_FLOAT_EQ(key.m_value, test_key.m_value);
	}

}

// + multitrack
