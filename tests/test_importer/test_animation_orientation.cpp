#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <gtest/gtest.h>

#include "Assert.h"

#include "utils/reference.h"
#include "render/Scene.h"

#include "importerSuite.h"
#include "Environment.h"

#define DEG_RAD (3.14159265359/180.)
#define RAD_DEG (180./3.14159265359)

#define KEY (20.f/60.f)

const struct {
	float key;
	float3 rotate;
} keyTable[] = {
	0.f * KEY, float3(180 * DEG_RAD, 0, 0),
	1.f * KEY, float3(180 * DEG_RAD, 45 * DEG_RAD, 0),

	2.f * KEY, float3(180 * DEG_RAD, 0, 0),
	3.f * KEY, float3(180 * DEG_RAD, 0, 90 * DEG_RAD),

	4.f * KEY, float3(180 * DEG_RAD, 0, 0),
	5.f * KEY, float3(180 * DEG_RAD, 90 * DEG_RAD, 0),

	6.f * KEY, float3(180 * DEG_RAD, 0, 0),
};

using namespace Grafkit;
using namespace GKimporter;

class ImportAnimationTest : public testing::Test {
public:
	ImportAnimationTest() {
		suite = nullptr;
	}

	~ImportAnimationTest() {
	}

	void SetUp() {
		suite = new ImporterSuite();
		suite->Execute("models/test_rot.blend");
		suite->Build();

		// decompose 
		scene = this->suite->Env()->GetBuilder().GetScene();
		if (scene.Invalid() || scene->Invalid())
			return;

		root = (*scene)->GetRootNode();
	}

	void TearDown() {
		if (suite)
			delete suite;
		suite = nullptr;
	}

public:
	ImporterSuite * suite;

	SceneResRef scene;
	ActorRef root;
};

TEST_F(ImportAnimationTest, orientation)
{
	ASSERT_TRUE(scene.Valid());
	ASSERT_TRUE(scene->Valid());

	//given
	ActorRef cube = (*scene)->GetNode("Cube");

	ASSERT_TRUE(cube.Valid());

	// when
	for (int i = 0; i < 6; i++) {
		float3 loc;
		Quaternion rot;
		float3 scale;

		(*scene)->UpdateAnimation(keyTable[i].key);
		Matrix mat0 = cube->WorldMatrix();

		mat0.Decompose(loc, rot, scale);

		float4 expectedRotAxis = Quaternion::FromEuler(keyTable[i].rotate).ToAxisAngle();
 		float4 actualRotAxis = rot.ToAxisAngle();

		ASSERT_FLOAT4_EQ(expectedRotAxis, actualRotAxis);
	}
}

