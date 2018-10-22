#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>
#include <fstream>
#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"
#include "utils/persistence/dynamics.h"
#include "utils/persistence/archive.h"
#include "assert.h"

using namespace Grafkit;

/* ****************************************************************************************************************************************
*
**************************************************************************************************************************************** */

class SimpleClass : public Serializable, virtual public Referencable
{
public:
    SimpleClass() : m_integer(0) {
    }


    SimpleClass(int integer, const std::string& string)
        : m_integer(integer)
        , m_string(string) {
    }

    int GetInteger() const { return m_integer; }
    void SetInteger(const int integer) { m_integer = integer; }
    std::string GetString() const { return m_string; }
    void SetString(const std::string& string) { m_string = string; }

private:
    int m_integer;
    std::string m_string;

    SERIALIZE(SimpleClass, 1, ar) { ar & m_integer & m_string; }
};

PERSISTENT_IMPL(SimpleClass);

TEST(Persistence, SimpleObjectTest)
{
    // given
    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));
    Ref<SimpleClass> obj = new SimpleClass(42, "some parameter");

    // when 
    a << obj;

    // then 
    Ref<SimpleClass> readObj;
    a >> readObj;

    ASSERT_TRUE(readObj);
    ASSERT_NE(obj, readObj);

    ASSERT_EQ(obj->GetInteger(), readObj->GetInteger());
    ASSERT_STREQ(obj->GetString().c_str(), readObj->GetString().c_str());
}

/* ****************************************************************************************************************************************
*
**************************************************************************************************************************************** */

class NestedClass : public Serializable, virtual public Referencable
{
public:
    NestedClass() {
    }

    NestedClass(const Ref<SimpleClass>& simpleClass, const Ref<SimpleClass>& simpleClass1)
        : m_obj1(simpleClass)
        , m_obj2(simpleClass1) {
    }

    Ref<SimpleClass> GetObj1() const { return m_obj1; }
    Ref<SimpleClass> GetObj2() const { return m_obj2; }

    SERIALIZE(NestedClass, 1, ar) { ar & m_obj1 & m_obj2; }

private:
    Ref<SimpleClass> m_obj1;
    Ref<SimpleClass> m_obj2;
};

PERSISTENT_IMPL(NestedClass);


TEST(Persistence, NestedClassTest)
{
    // given
    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));
    Ref<NestedClass> obj = new NestedClass(new SimpleClass(666, "Devil"), new SimpleClass(42, "The ultimate answer"));
    Ref<NestedClass> objNull = new NestedClass(nullptr, nullptr);

    // when 
    a << obj << objNull;

    // then 
    Ref<NestedClass> readObj, readObjNull;
    a >> readObj >> readObjNull;

    ASSERT_TRUE(readObj);
    ASSERT_TRUE(readObjNull);
    ASSERT_NE(obj, readObj);
    ASSERT_NE(objNull, readObjNull);

    // ... MORE ASSERTS

    ASSERT_FALSE(readObjNull->GetObj1());
    ASSERT_FALSE(readObjNull->GetObj2());
}

TEST(Persistence, BadTypeTest)
{
    // given
    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));
    Ref<SimpleClass> obj = new SimpleClass(42, "some parameter");

    // when 
    a << obj;

    // then 
    Ref<NestedClass> readObj;

    // will it blend?
    ASSERT_THROW(a >> readObj, std::exception);
}


/* ****************************************************************************************************************************************
 *
 **************************************************************************************************************************************** */

class SimpleBaseClass : public Serializable, public Referencable
{
public:

    SimpleBaseClass() : m_i(0) {
    }

    SimpleBaseClass(int i, const std::string& str)
        : m_i(i)
        , m_str(str) {
    }

    int GetI() const { return m_i; }
    std::string GetStr() const { return m_str; }

    virtual std::string GetSomeIntern() const = 0;

protected:
    template <class AR>
    void Serialize(AR& ar) { ar & m_i & m_str; }

private:
    int m_i;
    std::string m_str;
};

class DerivedClassA : public SimpleBaseClass
{
public:
    DerivedClassA() {
    }

    DerivedClassA(int i, const std::string& str, const std::string& str1)
        : SimpleBaseClass(i, str)
        , m_str1(str1) {
    }

    std::string GetSomeIntern() const override { return m_str1; }

    SERIALIZE(DerivedClassA, 1, ar)
    {
        SimpleBaseClass::Serialize(ar);
        ar & m_str1;
    }

private:
    std::string m_str1;
};

PERSISTENT_IMPL(DerivedClassA);

class DerivedClassB : public SimpleBaseClass
{
public:
    DerivedClassB() {
    }

    DerivedClassB(int i, const std::string& str, const std::string& str2)
        : SimpleBaseClass(i, str)
        , m_str2(str2) {
    }

    std::string GetSomeIntern() const override { return m_str2; }

    SERIALIZE(DerivedClassB, 1, ar)
    {
        SimpleBaseClass::Serialize(ar);
        ar & m_str2;
    }

private:
    std::string m_str2;
};

PERSISTENT_IMPL(DerivedClassB);

TEST(Persistence, PolimorphClassTest)
{
    // given
    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));
    Ref<SimpleBaseClass> objA = new DerivedClassA(42, "Hello", "World");
    Ref<SimpleBaseClass> objB = new DerivedClassB(666, "This is a", "test message");

    // when 
    a << objA << objB;

    // then 
    Ref<SimpleBaseClass> readObjA, readObjB;
    a >> readObjA >> readObjB;

    ASSERT_TRUE(readObjA);
    ASSERT_TRUE(readObjB);
    ASSERT_NE(objA, readObjA);
    ASSERT_NE(objB, readObjB);

    ASSERT_TRUE(readObjA);
    ASSERT_TRUE(dynamic_cast<DerivedClassA *>(readObjA.Get()));
    ASSERT_TRUE(objA->GetI() == readObjA->GetI());
    ASSERT_TRUE(objA->GetStr().compare(readObjA->GetStr()) == 0);
    ASSERT_TRUE(objA->GetSomeIntern().compare(readObjA->GetSomeIntern()) == 0);

    ASSERT_TRUE(readObjB);
    ASSERT_TRUE(dynamic_cast<DerivedClassB *>(readObjB.Get()));
    ASSERT_TRUE(objB->GetI() == readObjB->GetI());
    ASSERT_TRUE(objB->GetStr().compare(readObjB->GetStr()) == 0);
    ASSERT_TRUE(objB->GetSomeIntern().compare(readObjB->GetSomeIntern()) == 0);
}

void Verify(const Ref<SimpleBaseClass> &expected, const Ref<SimpleBaseClass> &actual)
{
    ASSERT_TRUE(actual);
    ASSERT_NE(expected, actual);
    ASSERT_EQ(expected->GetI(), actual->GetI());
    ASSERT_STREQ(expected->GetStr().c_str(), actual->GetStr().c_str());
    ASSERT_STREQ(expected->GetSomeIntern().c_str(), actual->GetSomeIntern().c_str());
}

TEST(Persistence, STLContainerObjects)
{
    // given
    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));

    std::array<Ref<SimpleBaseClass>, 256> array;
    std::vector<Ref<SimpleBaseClass>> vector;
    std::list<Ref<SimpleBaseClass>> list;
    for (size_t i = 0; i < 256; ++i)
    {
        array[i] = new DerivedClassA(42 + i, "Hello", "Array");
        vector.push_back(new DerivedClassB(64 + i, "Hello", "List"));
        list.push_back(new DerivedClassB(128 + i, "Hello", "List"));
    }

    // when
    a << array << vector << list;

    // then
    std::array<Ref<SimpleBaseClass>, 256> readArray;
    std::vector<Ref<SimpleBaseClass>> readVector;
    std::list<Ref<SimpleBaseClass>> readList;

    a >> readArray >> readVector >> readList;

    ASSERT_FALSE(readArray.empty());
    ASSERT_FALSE(readVector.empty());
    ASSERT_FALSE(readList.empty());

    // ... + asserts

    auto readIt =
        readList.begin();
    auto it = list.begin();
    for (size_t i = 0; i < 256; ++i)
    {
        Verify(array[i], readArray[i]);
        Verify(vector[i], readVector[i]);
        Verify(*it, *readIt);
        ++it, ++readIt;
    }
}

// TODO (1) + map
// TODO (1) + pair 

// TODO (1) + queue 
// TODO (1) + set, multiset
// TODO (1) + multimap 
