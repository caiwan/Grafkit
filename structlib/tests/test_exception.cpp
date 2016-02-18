#include <cstring>

#include "gtest/gtest.h"
#include "exceptions.h"

using namespace FWdebugExceptions;

void throw_nullpointer_exception() {
	throw EX(NullPointerException);
}

TEST(Exception, Throw)
{
	EXPECT_THROW(throw_nullpointer_exception(), NullPointerException);
}

void throw_nullpointer_exception_details(const char *what) {
	throw EX_DETAILS(NullPointerException, what);
}

TEST(Exception, ThrowDetails)
{
	const char *message = "test message";
	try {
		throw_nullpointer_exception_details(message);
		FAIL();
	}
	catch (NullPointerException &e) {
		const char *result = e.what();
		//ASSERT_NE(NULL, result);

		const char *pch = strstr(result, message);

		//ASSERT_NE(NULL, pch);
		ASSERT_STREQ(message, pch);
	}
	catch (...) {
		FAIL();
	}
}
