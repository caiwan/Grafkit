#include "stdafx.h"
#include "testLogger.h"

#include <gtest/gtest.h>

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

int main(int argc, char **argv) {
    Grafkit::Log::Logger().AddHandler(std::make_shared<TestLoggerHandler>());

	InitGoogleTest(&argc, argv);
	int res = RUN_ALL_TESTS();

#ifdef _DEBUG
	system("pause");
#endif // _DEBUG
	return res;
}
