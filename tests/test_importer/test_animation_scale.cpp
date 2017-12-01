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
	float3 scale;
} keyTable[] = {
	0.f * KEY, float3(0,0,0),
	1.f * KEY, float3(3,0,0),

	2.f * KEY, float3(0,0,0),
	3.f * KEY, float3(0,0,3),

	4.f * KEY, float3(0,0,0),
	5.f * KEY, float3(0,3,0),

	6.f * KEY, float3(0,0,0),
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
		suite->Execute("models/test_scale.blend");
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

TEST_F(ImportAnimationTest, scaling)
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

		ASSERT_FLOAT3_EQ(keyTable[i].scale, scale);
	}
}

