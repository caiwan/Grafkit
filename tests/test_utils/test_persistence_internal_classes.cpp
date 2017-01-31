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

#define D_TEST_FLOAT(f) (f)
#define D_TEST_FLOAT2(f) (float2((f), (f)))
#define D_TEST_FLOAT3(f) (float3((f), (f), (f)))
#define D_TEST_FLOAT4(f) (float4((f), (f), (f), (f)))
#define D_TEST_QUATERNION(f) (Quaternion::fromEuler((f), (f), (f)))

#define D_TEST_KEY(in, key, value, type)\
	in.m_key = key; in.m_type = type; in.m_value = value

#define ASSERT_KEY_EQ(val1, val2)\
	ASSERT_EQ(val1.m_type, val2.m_type); \
	ASSERT_FLOAT_EQ(val1.m_key, val2.m_key); \

TEST(Persistent_Animation, given_AnimationKey_when_persist_then_Load) {
	TestArchiver ar(256, true);

	// given
	Animation::FloatKey floatKey; D_TEST_KEY(floatKey, D_TEST_FLOAT(.1), D_TEST_FLOAT(.1), Animation::KI_catmullRom);
	Animation::Vector2Key float2Key; D_TEST_KEY(float2Key, D_TEST_FLOAT(.1), D_TEST_FLOAT2(.1), Animation::KI_ramp);
	Animation::Vector3Key float3Key; D_TEST_KEY(float3Key, D_TEST_FLOAT(.1), D_TEST_FLOAT3(.1), Animation::KI_smooth);
	Animation::Vector4Key float4Key; D_TEST_KEY(float4Key, D_TEST_FLOAT(.1), D_TEST_FLOAT4(.1), Animation::KI_step);

	//when
	PERSIST_FIELD(ar, floatKey);
	PERSIST_FIELD(ar, float2Key);
	PERSIST_FIELD(ar, float3Key);
	PERSIST_FIELD(ar, float4Key);

	//then
	ar.ResetCrsr();
	ar.SetDirection(false);

	Animation::FloatKey test_floatKey;
	Animation::Vector2Key test_float2Key;
	Animation::Vector3Key test_float3Key;
	Animation::Vector4Key test_float4Key;

	PERSIST_FIELD(ar, test_floatKey);
	PERSIST_FIELD(ar, test_float2Key);
	PERSIST_FIELD(ar, test_float3Key);
	PERSIST_FIELD(ar, test_float4Key);

	ASSERT_KEY_EQ(floatKey, test_floatKey);
	ASSERT_FLOAT_EQ(floatKey.m_value, test_floatKey.m_value);

	ASSERT_KEY_EQ(float2Key,test_float2Key);
	ASSERT_FLOAT2_EQ(float2Key.m_value, test_float2Key.m_value);
	
	ASSERT_KEY_EQ(float3Key, test_float3Key);
	ASSERT_FLOAT3_EQ(float3Key.m_value, test_float3Key.m_value);
	
	ASSERT_KEY_EQ(float4Key, test_float4Key);
	ASSERT_FLOAT4_EQ(float4Key.m_value, test_float4Key.m_value);
}


/*
	Tests if every used animation track types could've been persisted
*/

TEST(Persistent_Animation, given_AnimationTrack_when_persist_then_Load) {
	TestArchiver ar(16*4096, true);

	// given
	Animation::FloatTrack floatTrack;
	Animation::Vector2Track vector2Track;
	Animation::Vector3Track vector3Track;
	Animation::Vector4Track vector4Track;

	for (int i = 0; i < 256; ++i) {
		float f = 1. / i;
		Animation::FloatKey floatKey; D_TEST_KEY(floatKey, D_TEST_FLOAT(f), D_TEST_FLOAT(f), Animation::KI_catmullRom);
		Animation::Vector2Key vector2Key; D_TEST_KEY(vector2Key, D_TEST_FLOAT(f), D_TEST_FLOAT2(f), Animation::KI_ramp);
		Animation::Vector3Key vector3Key; D_TEST_KEY(vector3Key, D_TEST_FLOAT(f), D_TEST_FLOAT3(f), Animation::KI_smooth);
		Animation::Vector4Key vector4Key; D_TEST_KEY(vector4Key, D_TEST_FLOAT(f), D_TEST_FLOAT4(f), Animation::KI_step);

		floatTrack.AddKey(floatKey);
		vector2Track.AddKey(vector2Key);
		vector3Track.AddKey(vector3Key);
		vector4Track.AddKey(vector4Key);
	}
	
	//when
	floatTrack.serialize(ar);
	vector2Track.serialize(ar);
	vector3Track.serialize(ar);
	vector4Track.serialize(ar);

	//then
	ar.ResetCrsr();
	ar.SetDirection(false);

	Animation::FloatTrack test_floatTrack;
	Animation::Vector2Track test_vector2Track;
	Animation::Vector3Track test_vector3Track;
	Animation::Vector4Track test_vector4Track;

	test_floatTrack.serialize(ar);
	test_vector2Track.serialize(ar);
	test_vector3Track.serialize(ar);
	test_vector4Track.serialize(ar);

	ASSERT_EQ(floatTrack.GetKeyCount(), test_floatTrack.GetKeyCount());
	ASSERT_EQ(vector2Track.GetKeyCount(), test_vector2Track.GetKeyCount());
	ASSERT_EQ(vector3Track.GetKeyCount(), test_vector3Track.GetKeyCount());
	ASSERT_EQ(vector4Track.GetKeyCount(), test_vector4Track.GetKeyCount());

	for (int i = 0; i < 256; ++i) {
		Animation::FloatKey floatKey = floatTrack.GetKey(i);
		Animation::Vector2Key vector2Key = vector2Track.GetKey(i);
		Animation::Vector3Key vector3Key = vector3Track.GetKey(i);
		Animation::Vector4Key vector4Key = vector4Track.GetKey(i);

		Animation::FloatKey test_floatKey = test_floatTrack.GetKey(i);
		Animation::Vector2Key test_vector2Key = test_vector2Track.GetKey(i);
		Animation::Vector3Key test_vector3Key = test_vector3Track.GetKey(i);
		Animation::Vector4Key test_vector4Key = test_vector4Track.GetKey(i);

		ASSERT_KEY_EQ(floatKey, test_floatKey);
		ASSERT_FLOAT_EQ(floatKey.m_value, test_floatKey.m_value);

		ASSERT_KEY_EQ(vector2Key, test_vector2Key);
		ASSERT_FLOAT2_EQ(vector2Key.m_value, test_vector2Key.m_value);

		ASSERT_KEY_EQ(vector3Key, test_vector3Key);
		ASSERT_FLOAT3_EQ(vector3Key.m_value, test_vector3Key.m_value);

		ASSERT_KEY_EQ(vector4Key, test_vector4Key);
		ASSERT_FLOAT4_EQ(vector4Key.m_value, test_vector4Key.m_value);
	}

}

