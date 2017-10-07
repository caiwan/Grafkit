#include "stdafx.h"

#include <gtest/gtest.h>

#include "importerSuite.h"

#include "utils/reference.h"

#include "Environment.h"
#include "render/Scene.h"

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
		suite->Execute("build_test_scene_1");
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

TEST_F(ImportSchemaTest, given_ImportedScenegraph_when_checkSchema_then_success)
{
	// given
	int cameraCount = (*scene)->GetCameraCount();
	int lightCount = (*scene)->GetLightCount();

	ActorRef cube = (*scene)->GetNode("Cube_001");

	//when

	// then
	ASSERT_FALSE(scene.Invalid());
	ASSERT_FALSE(scene->Invalid());

	ASSERT_FALSE(root.Invalid());
	ASSERT_EQ(3, root->GetChildrenCount());
	
	ASSERT_EQ(1, cameraCount);
	ASSERT_EQ(1, lightCount);

	ASSERT_FALSE(cube.Invalid());
}


