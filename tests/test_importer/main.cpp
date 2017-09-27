#include "stdafx.h"

#include "ImporterTool.h"
#include "BlenderThread.h"

#include <gtest/gtest.h>

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

class ImporterIntegrationTest : public GKimporter::ImporterTool {
public:
	ImporterIntegrationTest() : ImporterTool() {

	}

	int Execute(int argc, char **argv) {
		int result = 0;

		InitGoogleTest(&argc, argv);

		result = SetupBlender();
		if (result != 0)
			return result;

		result = RUN_ALL_TESTS();

		blenderThread->Join();

		return result;
	}

	int SetupBlender() {
		// ... 
		blenderThread->Start();

		if (!blenderThread->IsRunning())
			return 1;

		return 0;
	}

};

int main(int argc, char **argv) {
	ImporterIntegrationTest test;

	int res = test.Execute(argc, argv);

#ifdef _DEBUG
	system("pause");
#endif // _DEBUG
	return res;
}
