#include "stdafx.h"



#include "utils/logger.h"

#include <gtest/gtest.h>

using namespace Grafkit;

using ::testing::InitGoogleTest;

class ImporterIntegrationTest {
public:
	ImporterIntegrationTest(){

	}

	int Execute(int argc, char **argv) {
		int result = 0;

#ifndef LIVE_RELEASE
		Log::Logger().SetLogLevel(Logger::LOG_DEBUG, false);
		Log::Logger().SetLogLevel(Logger::LOG_TRACE, false);
		Log::Logger().SetLogLevel(Logger::LOG_INFO, false);
		Log::Logger().SetLogLevel(Logger::LOG_WARN, true);
		Log::Logger().SetLogLevel(Logger::LOG_ERROR, true);
		Log::Logger().AddHandler(new LoggerHandler::ConsoleLogger());
		Log::Logger().AddHandler(new LoggerHandler::FileLoggerHandler("log.log", "error.log"));
#endif

		InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
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
