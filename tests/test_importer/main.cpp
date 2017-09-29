#include "stdafx.h"

#include "ImporterTool.h"
#include "BlenderThread.h"
#include "ServerThread.h"
#include "DispatcherThread.h"

#include "utils/logger.h"

#include <gtest/gtest.h>

using namespace Grafkit;

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

#ifndef LIVE_RELEASE
		Log::Logger().AddHandler(new LoggerHandler::ConsoleLogger());
		Log::Logger().AddHandler(new LoggerHandler::FileLoggerHandler("log.log", "error.log"));
#endif

		InitGoogleTest(&argc, argv);
		Setup();
		Start();

		// TODO: check if blender exited first

		Join();

		return RUN_ALL_TESTS();
	}

	void Setup() {

#ifndef LIVE_RELEASE
		blender->SetScriptRoot(IMPORTER_PY_ROOT);
#endif
		blender->AddArgument("host", GetHost());

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
