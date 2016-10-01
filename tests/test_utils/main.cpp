#include "stdafx.h"
#include "gtest/gtest.h"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

int main(int argc, char **argv) {
	InitGoogleTest(&argc, argv);

	//bool check_for_leaks = false;
	//if (argc > 1 && strcmp(argv[1], "--check_for_leaks") == 0)
	//	check_for_leaks = true;
	//else
	//	printf("%s\n", "Run this program with --check_for_leaks to enable "
	//		"custom leak checking in the tests.");

	//// If we are given the --check_for_leaks command line flag, installs the
	//// leak checker.
	//if (check_for_leaks) {
	//	TestEventListeners& listeners = UnitTest::GetInstance()->listeners();

	//	// listeners.Append(new LeakChecker);
	//}

	int res = RUN_ALL_TESTS();

#ifdef _DEBUG
	system("pause");
#endif // _DEBUG
	return res;
}
