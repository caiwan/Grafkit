#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include <cmath>

#include "utils/persistence/dynamics.h"
#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"


using namespace Grafkit;
using namespace FWdebugExceptions;

TEST(Persistence, given_Field_when_Persist_then_Load) {
	TestArchiver archive(6, true);
	
	int test = 0xfacababa;
	archive & new PersistField<decltype(test)>("test", test);

	archive.resetCrsr();
	archive.setDirection(false);

	int test_read = 0;
	archive & new PersistField<decltype(test)>("test", test_read);

	ASSERT_EQ(test, test_read);
}

/// @TODO test with more tzpes to see if it works 
TEST(Persistence, given_Field_when_PersistWithMacro_then_Load) {
	TestArchiver archive(6, true);

	int test = 0xfacababa;
	
	archive & PERSIST_FIELD(test);

	archive.resetCrsr();
	archive.setDirection(false);

	int test_orig = test;
	archive & PERSIST_FIELD(test);

	ASSERT_EQ(test_orig, test);
}

TEST(Persistence, given_FieldFloat234_when_PersistWithMacro_then_Load) {
	FAIL();
}

TEST(Persistence, given_FieldQuaternion_when_PersistWithMacro_then_Load) {
	FAIL();
}


TEST(Persistence, given_FieldMatrix_when_PersistWithMacro_then_Load) {
	FAIL();
}

TEST(Persistence, given_Vector_when_Persist_then_Load) {
	TestArchiver archive(4096, true);

	size_t len = rand() % 1024;

	int *test = new int[len];

	for (size_t i = 0; i < len; i++) {
		test[i] = rand();
	}

	archive & new PersistVector<std::remove_pointer<decltype(test)>::type>("test", test, len);

	archive.resetCrsr();
	archive.setDirection(false);

	int *test_read = nullptr;
	size_t len_read = 0;
	archive & new PersistVector<std::remove_pointer<decltype(test)>::type>("test", test_read, len_read);

	ASSERT_EQ(len, len_read);
	for (size_t i=0; i<len; i++)
		ASSERT_EQ(test[i], test_read[i]);

	delete[] test_read;
	delete[] test;
}

TEST(Persistence, given_Vector_when_PersistWithMacro_then_Load) {
	TestArchiver archive(4096, true);

	size_t len = rand() % 1024;
	int *test = new int[len];

	// @todo float2,3,4, quaternion, matrix

	for (size_t i = 0; i < len; i++) {
		test[i] = rand();
	}

	archive & PERSIST_VECTOR(test, len);

	archive.resetCrsr();
	archive.setDirection(false);

	size_t len_orig = len;
	int *test_orig = test;

	archive & PERSIST_VECTOR(test, len);

	ASSERT_FALSE(nullptr == test);

	ASSERT_EQ(len_orig, len);
	for (size_t i = 0; i<len; i++)
		ASSERT_EQ(test_orig[i], test[i]);

	delete[] test_orig;
	delete[] test;
}

TEST(Persistence, given_PVoidVector_when_Persist_then_Load) {
	FAIL();
}

TEST(Persistence, given_PVoidVector_when_PersistWithMacro_then_Load) {
	FAIL();
}

TEST(Persistence, given_String_when_Persist_then_Load) {
	const char *szTest = "The quick brown fox jumps over the lazy dog.";
	//const size_t nTest = strlen(szTest);

	TestArchiver archive(256, true);

	archive & new PersistString("test", &szTest);

	archive.resetCrsr();
	archive.setDirection(false);

	const char * szRead = nullptr;

	archive & new PersistString("test", &szRead);

	ASSERT_FALSE(nullptr == szRead);
	ASSERT_STREQ(szTest, szRead);

	delete[] szRead;
}

TEST(Persistence, given_STDString_when_Persist_then_Load) {
	std::string test = "The quick brown fox jumps over the lazy dog.";

	TestArchiver archive(256, true);

	archive & new PersistString("test", &test);

	archive.resetCrsr();
	archive.setDirection(false);

	std::string read;

	archive & new PersistString("test", &read);

	ASSERT_FALSE(read.empty());
	ASSERT_STREQ(test.c_str(), read.c_str());
}

TEST(Persistence, given_StdString_when_Persist_then_LoadToConstChar) {
	std::string test = "The quick brown fox jumps over the lazy dog.";
	const size_t nTest = test.length();

	TestArchiver archive(256, true);

	archive & new PersistString("test", &test);

	archive.resetCrsr();
	archive.setDirection(false);

	const char * szRead = nullptr;

	archive & new PersistString("test", &szRead);

	ASSERT_FALSE(nullptr == szRead);
	ASSERT_EQ(nTest, strlen(szRead));
	ASSERT_STREQ(test.c_str(), szRead);

	delete[] szRead;
}

TEST(Persistence, given_Object_when_Persist_then_Load) {
	TestArchiver archive(256, true);

	FAIL();

}

TEST(Persistence, given_ObjectWithFields_when_Persist_then_Load) {
	TestArchiver archive(256, true);

	FAIL();
}

TEST(Persistence, given_ObjectCascadeObjects_when_Persist_then_Load) {
	TestArchiver archive(256, true);

	FAIL();
}

TEST(Persistence, given_ObjectCascadeObjectsAndFields_when_Persist_then_Load) {
	TestArchiver archive(256, true);

	FAIL();
}
