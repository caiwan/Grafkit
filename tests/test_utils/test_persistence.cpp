#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include <cmath>

#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"


#include "utils/persistence/dynamics.h"
#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"


using namespace Grafkit;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;

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
	TestArchiver archive(256, true);
	
	// given
	float2 f2;
	f2.x = 1.0; f2.y = 2.16;

	float3 f3;
	f3.x = 1.0; f3.y = 2.16; f3.z = 3.14159;

	float4 f4;
	f4.x = 1.0; f4.y = 2.16; f4.z = 3.14159; f4.w = 4.0;

	//when
	archive & PERSIST_FIELD(f2);
	archive & PERSIST_FIELD(f3);
	archive & PERSIST_FIELD(f4);


	//then
	archive.setDirection(false);
	archive.resetCrsr();

	float2 ft2 = f2;
	float3 ft3 = f3;
	float4 ft4 = f4;
	
	archive & PERSIST_FIELD(f2);
	archive & PERSIST_FIELD(f3);
	archive & PERSIST_FIELD(f4);

	ASSERT_LT(abs(f2.x - ft2.x), 000001);
	ASSERT_LT(abs(f2.y - ft2.y), 000001);
	
	ASSERT_LT(abs(f3.x - ft3.x), 000001);
	ASSERT_LT(abs(f3.y - ft3.y), 000001);
	ASSERT_LT(abs(f3.z - ft3.z), 000001);
	
	ASSERT_LT(abs(f4.x - ft4.x), 000001);
	ASSERT_LT(abs(f4.y - ft4.y), 000001);
	ASSERT_LT(abs(f4.z - ft4.z), 000001);
	ASSERT_LT(abs(f4.w - ft4.w), 000001);
}

TEST(Persistence, given_FieldQuaternion_when_PersistWithMacro_then_Load) {
	TestArchiver archive(256, true);
	
	// given
	float4 f4;
	f4.x = 1.0; f4.y = 2.16; f4.z = 3.14159; f4.w = 4.0;
	Quaternion q = f4;
	
	//when
	archive & PERSIST_FIELD(q);
	
	//then
	archive.setDirection(false);
	archive.resetCrsr();

	Quaternion q2 = q;
	archive & PERSIST_FIELD(q);
	float4 ft4 = q2;

	ASSERT_LT(abs(f4.x - ft4.x), 000001);
	ASSERT_LT(abs(f4.y - ft4.y), 000001);
	ASSERT_LT(abs(f4.z - ft4.z), 000001);
	ASSERT_LT(abs(f4.w - ft4.w), 000001);
}


TEST(Persistence, given_FieldMatrix_when_PersistWithMacro_then_Load) {
	TestArchiver archive(256, true);

	//given
	Matrix m(
		11, 12, 13, 14,
		21, 22, 23, 24,
		31, 32, 33, 34,
		41, 42, 43, 44
	);

	//when
	archive & PERSIST_FIELD(m);

	//then
	archive.setDirection(false);
	archive.resetCrsr();

	Matrix m2 = m;

	archive & PERSIST_FIELD(m);

	for (size_t row = 0; row < 4; row++) {
		for (size_t col = 0; col < 4; col++) {
			ASSERT_LT(abs(m.Get(row,col) - m2.Get(row, col)), 000001);
		}
	}
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

	//given
	EmptyClass *object = new EmptyClass();
	
	//when
	archive & new PersistObject("test", (Persistent**)&object);

	//then
	archive.resetCrsr();
	archive.setDirection(false);

	EmptyClass *object_test = nullptr;

	archive & new PersistObject("test", (Persistent**)&object_test);

	ASSERT_TRUE(object != nullptr);
	ASSERT_TRUE(object != object_test);
	ASSERT_STREQ(object->getClassName().c_str(), object_test->getClassName().c_str());

	delete object;
}

TEST(Persistence, given_Object_when_PersistRefernce_then_Load) {
	TestArchiver archive(256, true);

	//given
	Ref<EmptyClass> object = new EmptyClass();

	//when
	archive & new PersistObject("test", (Persistent**)&object);

	//then
	archive.resetCrsr();
	archive.setDirection(false);

	Ref<EmptyClass> object_test = nullptr;

	archive & new PersistObject("test", (Persistent**)&object_test);

	ASSERT_TRUE(object_test.Valid());
	ASSERT_TRUE(object_test != object);
	ASSERT_STREQ(object->getClassName().c_str(), object_test->getClassName().c_str());
}

TEST(Persistence, given_Object_when_PersistWithMacro_then_Load) {
	TestArchiver archive(256, true);

	//given
	EmptyClass *object = new EmptyClass();

	//when
	archive & PERSIST_OBJECT(object);

	//then
	archive.resetCrsr();
	archive.setDirection(false);

	EmptyClass *object_original = object;

	object = nullptr;
	archive & PERSIST_OBJECT(object);


	ASSERT_TRUE(object != nullptr);
	ASSERT_TRUE(object != object_original);
	ASSERT_STREQ(object->getClassName().c_str(), object_original->getClassName().c_str());

	delete object;
}

TEST(Persistence, given_ObjectWithFields_when_Persist_then_Load) {
	TestArchiver archive(256, true);

	// given
	FieldClass *object = new FieldClass();
	object->buildup(0xfafababa, 2.16);

	// when
	archive & PERSIST_OBJECT(object);

	// then
	FieldClass *object_original = object;

	archive.resetCrsr();
	archive.setDirection(false);

	object = nullptr;
	archive & PERSIST_OBJECT(object);


	ASSERT_TRUE(object != nullptr);
	ASSERT_TRUE(object != object_original);
	ASSERT_STREQ(object->getClassName().c_str(), object_original->getClassName().c_str());
	ASSERT_TRUE((*object) == (*object_original));
}

TEST(Persistence, given_ObjectCascadeObjectsAndFields_when_Persist_then_Load) {
	TestArchiver archive(256, true);

	// given
	NestedClass *object = new NestedClass();
	object->buildup(0xfacababa, 2.16, 0x012345678, 3.141592);

	// when
	archive & PERSIST_OBJECT(object);

	// then
	NestedClass *object_original = object;

	archive.resetCrsr();
	archive.setDirection(false);

	object = nullptr;
	archive & PERSIST_OBJECT(object);


	ASSERT_TRUE(object != nullptr);
	ASSERT_TRUE(object != object_original);
	ASSERT_STREQ(object->getClassName().c_str(), object_original->getClassName().c_str());
	ASSERT_TRUE((*object) == (*object_original));
}

TEST(Persistence, given_NullPointer_when_Persist_then_Load) {
	TestArchiver archive(256, true);

	// given
	NestedClass *object = nullptr;

	// when
	archive & PERSIST_OBJECT(object);

	// then
	NestedClass *object_original = object;

	archive.resetCrsr();
	archive.setDirection(false);

	object = nullptr;
	archive & PERSIST_OBJECT(object);


	ASSERT_TRUE(object == nullptr);
}
