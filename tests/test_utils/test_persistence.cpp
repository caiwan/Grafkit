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

TEST(Persistence, PersistFieldTest)
{
    TestArchiver archive(6, true);
    // Given 

    int test = 0xfacababa;

    // When
    archive.PersistField<decltype(test)>(test);

    // Then
    archive.ResetCrsr();
    archive.SetDirection(false);

    int test_read = 0;
    archive.PersistField<decltype(test)>(test_read);

    ASSERT_EQ(test, test_read);
}

/// @TODO test with more tzpes to see if it works 
TEST(Persistence, PersitFieldMacroTest)
{
    TestArchiver archive(6, true);
    // Given

    int test = 0xfacababa;

    // When
    PERSIST_FIELD(archive, test);

    // Then
    archive.ResetCrsr();
    archive.SetDirection(false);

    int test_orig = test;
    PERSIST_FIELD(archive, test);

    ASSERT_EQ(test_orig, test);
}

TEST(Persistence, NDimensionFloatPersistTest)
{
    TestArchiver archive(256, true);

    // given
    float2 f2;
    f2.x = 1.0;
    f2.y = 2.16;

    float3 f3;
    f3.x = 1.0;
    f3.y = 2.16;
    f3.z = 3.14159;

    float4 f4;
    f4.x = 1.0;
    f4.y = 2.16;
    f4.z = 3.14159;
    f4.w = 4.0;

    //when
    PERSIST_FIELD(archive, f2);
    PERSIST_FIELD(archive, f3);
    PERSIST_FIELD(archive, f4);


    //then
    archive.SetDirection(false);
    archive.ResetCrsr();

    float2 ft2 = f2;
    float3 ft3 = f3;
    float4 ft4 = f4;

    PERSIST_FIELD(archive, f2);
    PERSIST_FIELD(archive, f3);
    PERSIST_FIELD(archive, f4);

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

TEST(Persistence, QuaternionPersistTest)
{
    TestArchiver archive(256, true);

    // given
    float4 f4;
    f4.x = 1.0;
    f4.y = 2.16;
    f4.z = 3.14159;
    f4.w = 4.0;
    Quaternion q = f4;

    //when
    PERSIST_FIELD(archive, q);

    //then
    archive.SetDirection(false);
    archive.ResetCrsr();

    Quaternion q2 = q;
    PERSIST_FIELD(archive, q);
    float4 ft4 = q2;

    ASSERT_LT(abs(f4.x - ft4.x), 000001);
    ASSERT_LT(abs(f4.y - ft4.y), 000001);
    ASSERT_LT(abs(f4.z - ft4.z), 000001);
    ASSERT_LT(abs(f4.w - ft4.w), 000001);
}


TEST(Persistence, MatrixPersistTest)
{
    TestArchiver archive(256, true);

    //given
    Matrix m(
        11, 12, 13, 14,
        21, 22, 23, 24,
        31, 32, 33, 34,
        41, 42, 43, 44
    );

    //when
    PERSIST_FIELD(archive, m);

    //then
    archive.SetDirection(false);
    archive.ResetCrsr();

    Matrix m2 = m;

    PERSIST_FIELD(archive, m);

    for (size_t row = 0; row < 4; row++)
    {
        for (size_t col = 0; col < 4; col++)
        {
            ASSERT_LT(abs(m.Get(row, col) - m2.Get(row, col)), 000001);
        }
    }
}

TEST(Persistence, VectorPersistTest)
{
    TestArchiver archive(4096, true);

    // given 
    size_t len = rand() % 1024;

    int* test = new int[len];

    for (size_t i = 0; i < len; i++) { test[i] = rand(); }

    // when
    archive.PersistVector<std::remove_pointer<decltype(test)>::type>(test, len);

    //then
    archive.ResetCrsr();
    archive.SetDirection(false);

    int* test_read = nullptr;
    size_t len_read = 0;
    archive.PersistVector<std::remove_pointer<decltype(test_read)>::type>(test_read, len_read);

    ASSERT_EQ(len, len_read);
    for (size_t i = 0; i < len; i++)
    ASSERT_EQ(test[i], test_read[i]);

    delete[] test_read;
    delete[] test;
}

TEST(Persistence, VectorMacroPersistTest)
{
    TestArchiver archive(4096, true);

    // given
    size_t len = rand() % 1024;
    int* test = new int[len];

    // @todo float2,3,4, quaternion, matrix

    for (size_t i = 0; i < len; i++) { test[i] = rand(); }

    PERSIST_VECTOR(archive, test, len);

    archive.ResetCrsr();
    archive.SetDirection(false);

    size_t len_orig = len;
    int* test_orig = test;

    PERSIST_VECTOR(archive, test, len);

    ASSERT_FALSE(nullptr == test);

    ASSERT_EQ(len_orig, len);
    for (size_t i = 0; i < len; i++)
    ASSERT_EQ(test_orig[i], test[i]);

    delete[] test_orig;
    delete[] test;
}

TEST(Persistence, StringPersistTest)
{
    TestArchiver archive(256, true);

    // given
    const char* szTest = "The quick brown fox jumps over the lazy dog.";

    // when
    archive.PersistString(szTest);

    // then
    archive.ResetCrsr();
    archive.SetDirection(false);

    const char* szRead = nullptr;

    archive.PersistString(szRead);

    ASSERT_FALSE(nullptr == szRead);
    ASSERT_STREQ(szTest, szRead);

    delete[] szRead;
}

TEST(Persistence, STDStringPerstTest)
{
    TestArchiver archive(256, true);

    // given
    std::string test = "The quick brown fox jumps over the lazy dog.";

    // when 
    archive.PersistString(test);

    // then
    archive.ResetCrsr();
    archive.SetDirection(false);

    std::string read;

    archive.PersistString(read);

    ASSERT_FALSE(read.empty());
    ASSERT_STREQ(test.c_str(), read.c_str());
}

TEST(Persistence, STDStringToCstringDeserializeTest)
{
    TestArchiver archive(256, true);

    // given
    std::string test = "The quick brown fox jumps over the lazy dog.";
    const size_t nTest = test.length();

    // when
    archive.PersistString(test);

    archive.ResetCrsr();
    archive.SetDirection(false);

    const char* szRead = nullptr;

    archive.PersistString(szRead);

    ASSERT_FALSE(nullptr == szRead);
    ASSERT_EQ(nTest, strlen(szRead));
    ASSERT_STREQ(test.c_str(), szRead);

    delete[] szRead;
}

TEST(Persistence, ObjectPersistTest)
{
    TestArchiver archive(256, true);

    //given
    EmptyClass* object = new EmptyClass();

    //when
    archive.StoreObject(object);

    //then
    archive.ResetCrsr();
    archive.SetDirection(false);

    EmptyClass* object_test = nullptr;

    object_test = dynamic_cast<decltype(object_test)>(archive.LoadObject());

    ASSERT_TRUE(object_test != nullptr);
    ASSERT_TRUE(object != object_test);
    ASSERT_STREQ(object->GetClazzName().c_str(), object_test->GetClazzName().c_str());

    delete object;
}

TEST(Persistence, ObjectReferencePersistTest)
{
    TestArchiver archive(256, true);

    //given
    Ref<EmptyClass> object = new EmptyClass();

    //when
    archive.StoreObject(object);

    //then
    archive.ResetCrsr();
    archive.SetDirection(false);

    Ref<EmptyClass> object_test = nullptr;
    object_test = dynamic_cast<decltype(object_test.Get())>(archive.LoadObject());

    ASSERT_TRUE(object_test.Valid());
    ASSERT_TRUE(object_test != object);
    ASSERT_STREQ(object->GetClazzName().c_str(), object_test->GetClazzName().c_str());
}

TEST(Persistence, ObjectReferenceMacroPeristTest)
{
    TestArchiver archive(256, true);

    //given
    Ref<EmptyClass> object = new EmptyClass();

    //when
    PERSIST_REFOBJECT(archive, object);

    //then
    archive.ResetCrsr();
    archive.SetDirection(false);

    Ref<EmptyClass> object_read = nullptr;
    PERSIST_REFOBJECT(archive, object_read);

    ASSERT_TRUE(object_read.Valid());
    ASSERT_TRUE(object_read != object);
    ASSERT_STREQ(object->GetClazzName().c_str(), object_read->GetClazzName().c_str());
}


TEST(Persistence, ObjectMacroPersistTest)
{
    TestArchiver archive(256, true);

    //given
    EmptyClass* object = new EmptyClass();

    //when
    PERSIST_OBJECT(archive, object);

    //then
    archive.ResetCrsr();
    archive.SetDirection(false);

    EmptyClass* object_read = nullptr;

    PERSIST_OBJECT(archive, object_read);


    ASSERT_TRUE(object_read != nullptr);
    ASSERT_TRUE(object_read != object);
    ASSERT_STREQ(object_read->GetClazzName().c_str(), object->GetClazzName().c_str());

    delete object;
}

TEST(Persistence, ObjectWithFieldsPersistTest)
{
    TestArchiver archive(256, true);

    // given
    FieldClass* object = new FieldClass();
    object->Build(0xfafababa, 2.16);

    // when
    PERSIST_OBJECT(archive, object);

    // then
    FieldClass* object_original = object;

    archive.ResetCrsr();
    archive.SetDirection(false);

    object = nullptr;
    PERSIST_OBJECT(archive, object);


    ASSERT_TRUE(object != nullptr);
    ASSERT_TRUE(object != object_original);
    ASSERT_STREQ(object->GetClazzName().c_str(), object_original->GetClazzName().c_str());
    ASSERT_TRUE((*object) == (*object_original));
}

TEST(Persistence, ObjectCascadeObjectsPersistTest)
{
    TestArchiver archive(256, true);

    // given
    NestedClass* object = new NestedClass();
    object->Buildup(0xfacababa, 2.16, 0x012345678, 3.141592);

    // when
    PERSIST_OBJECT(archive, object);

    // then
    NestedClass* object_original = object;

    archive.ResetCrsr();
    archive.SetDirection(false);

    object = nullptr;
    PERSIST_OBJECT(archive, object);

    ASSERT_TRUE(object != nullptr);
    ASSERT_TRUE(object != object_original);
    ASSERT_STREQ(object->GetClazzName().c_str(), object_original->GetClazzName().c_str());
    ASSERT_TRUE((*object) == (*object_original));
}

TEST(Persistence, NullPointerPersistTest)
{
    TestArchiver archive(256, true);

    // given
    NestedClass* object = nullptr;

    // when
    PERSIST_OBJECT(archive, object);

    // then
    NestedClass* object_original = object;

    archive.ResetCrsr();
    archive.SetDirection(false);

    object = nullptr;
    PERSIST_OBJECT(archive, object);

    ASSERT_TRUE(object == nullptr);
}
