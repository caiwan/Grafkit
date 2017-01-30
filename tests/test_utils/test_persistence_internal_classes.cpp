/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>
#include <cmath>

#include "Assert.h"

#include "render/Scene.h"
#include "render/Model.h"
#include "render/Animation.h"

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

DEFINE_PREGNANCY_TEST(Scene)
DEFINE_PREGNANCY_TEST(Actor)
DEFINE_PREGNANCY_TEST(Material)
DEFINE_PREGNANCY_TEST(Mesh)
DEFINE_PREGNANCY_TEST(Model)

/*
	Tests if every used animation key types could've been persisted
*/

#define D_TEST_FLOAT (.1)
#define D_TEST_FLOAT2 (float2(.1, .2))
#define D_TEST_FLOAT3 (float3(.1, .2, .3))
#define D_TEST_FLOAT4 (float4(.1, .2, .3, .4))
#define D_TEST_QUATERNION (Quaternion::fromEuler(.1, .2, .3))

#define D_TEST_KEY(in, key, value, type)\
	in.m_key = key; in.m_type = type; in.m_value = value

#define ASSERT_KEY_EQ(val1, val2)\
	ASSERT_EQ(val1.m_type, val2.m_type); \
	ASSERT_FLOAT_EQ(val1.m_key, val2.m_key); \

TEST(Persistent_Animation, given_AnimationKey_when_persist_then_Load) {
	TestArchiver ar(256, true);

	// given
	Animation::Key<float> floatKey; D_TEST_KEY(floatKey, D_TEST_FLOAT, D_TEST_FLOAT, Animation::KI_catmullRom);
	Animation::Key<float2> float2Key; D_TEST_KEY(float2Key, D_TEST_FLOAT, D_TEST_FLOAT2, Animation::KI_ramp);
	Animation::Key<float3> float3Key; D_TEST_KEY(float3Key, D_TEST_FLOAT, D_TEST_FLOAT3, Animation::KI_smooth);	
	Animation::Key<float4> float4Key; D_TEST_KEY(float4Key, D_TEST_FLOAT, D_TEST_FLOAT4, Animation::KI_step);
	Animation::Key<Quaternion> quaternionKey; D_TEST_KEY(quaternionKey, D_TEST_FLOAT, D_TEST_QUATERNION, Animation::KI_linear);

	//when
	PERSIST_FIELD(ar, floatKey);
	PERSIST_FIELD(ar, float2Key);
	PERSIST_FIELD(ar, float3Key);
	PERSIST_FIELD(ar, float4Key);
	PERSIST_FIELD(ar, quaternionKey);

	//then
	ar.ResetCrsr();
	ar.SetDirection(false);

	Animation::Key<float>      test_read_floatKey;
	Animation::Key<float2>     test_read_float2Key;
	Animation::Key<float3>     test_read_float3Key;
	Animation::Key<float4>     test_read_float4Key;
	Animation::Key<Quaternion> test_read_quaternionKey;

	PERSIST_FIELD(ar, test_read_floatKey);
	PERSIST_FIELD(ar, test_read_float2Key);
	PERSIST_FIELD(ar, test_read_float3Key);
	PERSIST_FIELD(ar, test_read_float4Key);
	PERSIST_FIELD(ar, test_read_quaternionKey);

	ASSERT_KEY_EQ(floatKey, test_read_floatKey);
	ASSERT_FLOAT_EQ(floatKey.m_value, test_read_floatKey.m_value);

	ASSERT_KEY_EQ(float2Key,test_read_float2Key);
	ASSERT_FLOAT2_EQ(float2Key.m_value, test_read_float2Key.m_value);
	
	ASSERT_KEY_EQ(float3Key, test_read_float3Key);
	ASSERT_FLOAT3_EQ(float3Key.m_value, test_read_float3Key.m_value);
	
	ASSERT_KEY_EQ(float4Key, test_read_float4Key);
	ASSERT_FLOAT4_EQ(float4Key.m_value, test_read_float4Key.m_value);
	
	ASSERT_KEY_EQ(quaternionKey, test_read_quaternionKey);
	float4 quaternion = quaternionKey.m_value, test_quaternion = quaternionKey.m_value;
	ASSERT_FLOAT4_EQ(quaternion, test_quaternion);
}


/*
	Tests if every used animation track types could've been persisted
*/

TEST(Persistent_Animation, given_AnimationTrack_when_persist_then_Load) {
	TestArchiver ar(256, true);
	FAIL();
}