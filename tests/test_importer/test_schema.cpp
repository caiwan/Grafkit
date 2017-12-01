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

using namespace Grafkit;
using namespace GKimporter;

class ImportSchemaTest : public testing::Test {
public:
	ImportSchemaTest() {
		suite = nullptr;
	}

	~ImportSchemaTest() {
	}

	void SetUp() {
		//suite = nullptr;

		suite = new ImporterSuite();
		suite->Execute();
		suite->Build();

		// decompose 
		scene = this->suite->Env()->GetBuilder().GetScene();
		if (scene.Invalid() || scene->Invalid())
			return;

		root = (*scene)->GetRootNode();
		if (root.Invalid())
			return;
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

TEST_F(ImportSchemaTest, checkCreatedScenegraphObjects)
{
	// given
	// blender default scene
	// containing 
	// - a single cube
	// - a camera
	// - point light

	//when
	int cameraCount = (*scene)->GetCameraCount();
	int lightCount = (*scene)->GetLightCount();

	ActorRef cube = (*scene)->GetNode("Cube");

	// then
	ASSERT_FALSE(scene.Invalid());
	ASSERT_FALSE(scene->Invalid());

	ASSERT_FALSE(root.Invalid());
	ASSERT_EQ(3, root->GetChildrenCount());

	ASSERT_EQ(1, cameraCount);
	ASSERT_EQ(1, lightCount);

	ASSERT_FALSE(cube.Invalid());
}


TEST_F(ImportSchemaTest, checkMatricesOfScenegraph)
{
	// given
	// blender default scene
	// containing 
	// - a single cube
	// - a camera
	// - point light

	//when
	ActorRef cube = (*scene)->GetNode("Cube");
	ActorRef camera = (*scene)->GetNode("Camera");
	ActorRef light = (*scene)->GetNode("Lamp");

	// then
	ASSERT_FALSE(scene.Invalid());
	ASSERT_FALSE(scene->Invalid());

	ASSERT_FALSE(root.Invalid());
	ASSERT_EQ(3, root->GetChildrenCount());

	// 
	ASSERT_FALSE(cube.Invalid());

	// TODO: assert cube 

	// --- camera
	ASSERT_FALSE(camera.Invalid());

	float3 cameraPos;
	Quaternion cameraOirentation;
	float3 cameraScale;
	ASSERT_TRUE(camera->Matrix().Decompose(cameraPos, cameraOirentation, cameraScale));
	
	ASSERT_FLOAT3_EQ(float3(7.48113, -6.50764, -5.34367), cameraPos);

	// Itt a LH-RH konvezyio kicsit furcsa
	Quaternion orientation = Quaternion::FromEuler(63.559*DEG_RAD, 0, 46.692*DEG_RAD);
	float4 orientationVector = orientation.ToAxisAngle();
	float4 cameraOrientationVector = cameraOirentation.ToAxisAngle();

	ASSERT_FLOAT4_EQ(orientationVector, cameraOrientationVector);

	// --- light
	ASSERT_FALSE(light.Invalid());

}


