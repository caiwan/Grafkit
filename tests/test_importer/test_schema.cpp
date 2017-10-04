#include "stdafx.h"

#include <gtest/gtest.h>

#include "importerSuite.h"

#include "utils/reference.h"

#include "Environment.h"
#include "render/Scene.h"

using namespace Grafkit;
using namespace GKimporter;

class ImportSchemaTest : public testing::TestCase {
public:
	ImportSchemaTest() {
		suite = nullptr;

		suite = new ImporterSuite();
		suite->Execute("schemaTest.py");
		suite->Build();

		// decompose 
		scene = this->suite->Env()->GetBuilder().GetScene();
		if (!scene.Invalid() || !scene->Invalid())
			return;

		root = (*scene)->GetRootNode();
		if (root.Invalid())
			return;
	}

	~ImportSchemaTest() {
		if (suite)
			delete suite;
		suite = nullptr;
	}

	void SetUp() {
	}

	void TearDown() {
	}

public:
	ImporterSuite * suite;

	SceneResRef scene;
	ActorRef root;
};

TEST_F(ImportSchemaTest, given_ImportedScenegraph_when_checkEmpty_then_success)
{
	// given
	//when

	// then
	ASSERT_FALSE(scene.Invalid());
	ASSERT_FALSE(scene->Invalid());

	ASSERT_FALSE(root.Invalid());
	ASSERT_NE(0, root->GetChildrenCount());
}

TEST_F(ImportSchemaTest, given_ImportedScenegraph_when_checkSchema_then_success)
{
	ASSERT_FALSE(root.Invalid());
	ASSERT_NE(0, root->GetChildrenCount());

	// given
	// when
	int cameraCount = (*scene)->GetCameraCount();
	int lightCount = (*scene)->GetLightCount();

	// then
	ASSERT_EQ(1, cameraCount);
	ASSERT_EQ(1, lightCount);
}