#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include <cmath>

#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"

#include "utils/persistence/dynamics.h"
#include "utils/persistence/Archive.h"

#include <fstream>

using namespace Grafkit;

struct Message
{
    int id;
    std::string name;
    std::vector<std::string> groups;
    float coolNumbers[3];

    Message(int nid = 1, const std::string& nname = "unknown") : id(nid)
        , name(nname)
    {
        coolNumbers[0] = 3.1416f;
        coolNumbers[1] = 2.7182f;
        coolNumbers[2] = 1.6180f;

        groups.push_back("wheel");
        groups.push_back("lp");
        groups.push_back("root");
        groups.push_back("sudo");
    }

    template <class T>
    void Serialize(T& archive)
    {
        archive & id;
        archive & name;
        archive & groups;
        archive & coolNumbers;
    }

    // equality operator for the tests
    bool operator==(const Message& m) const
    {
        if (id != m.id)
            return false;
        if (name != m.name)
            return false;
        for (int i = 0; i < 3; ++i)
            if (coolNumbers[i] != m.coolNumbers[i])
                return false;
        for (unsigned int i = 0; i < groups.size(); ++i)
            if (groups[i] != m.groups[i])
                return false;

        return true;
    }

    bool operator!=(const Message& m) const { return !(*this == m); }
};

template <class T>
void TestArchive(const T& testValue)
{
    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));
    T value(testValue);

    a << testValue;
    ASSERT_EQ(value, testValue);

    value = T();
    ASSERT_NE(value, testValue);

    a >> value;
    ASSERT_EQ(value, testValue);
}

template <class T, size_t N>
void TestArray(T (&testValue)[N])
{
    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));

    T (&value)[N](testValue);

    a << testValue;
    a >> value;

    for (size_t i = 0; i < N; ++i)
    ASSERT_EQ(value[i], testValue[i]);
}

// Folytkov
TEST(Archive, PODTest)
{
    // POD
    //std::cout << "POD..." << std::endl;
    TestArchive(true);
    TestArchive((char)123);
    TestArchive((unsigned char)123);
    TestArchive((short)123);
    TestArchive((unsigned short)123);
    TestArchive((int)123);
    TestArchive((unsigned int)123);
    TestArchive((long)123);
    TestArchive((unsigned long)123);
    TestArchive((long long)123);
    TestArchive((unsigned long long)123);
    TestArchive(std::string("salut"));
    TestArchive(123.456f);
    TestArchive(1.6180f);
    TestArchive(123.456);
}

TEST(Archive, ArrayTest)
{
    // Array
    std::cout << "Array..." << std::endl;
    int ii[] = {1, 2, 3, 4, 5, 6};
    TestArray(ii);
}

TEST(Archive, STLTest)
{
    // STL
    std::cout << "STL..." << std::endl;
    TestArchive(std::vector<int>({1, 2, 3, 4, 5}));
    TestArchive(std::vector<std::string>({"a", "bb", "ccc", "dddd"}));
    TestArchive(std::deque<int>({1, 2, 3, 4, 5}));
    TestArchive(std::deque<std::string>({"a", "bb", "ccc", "dddd"}));
    TestArchive(std::list<int>({1, 2, 3, 4, 5}));
    TestArchive(std::list<std::string>({"a", "bb", "ccc", "dddd"}));
    TestArchive(std::set<int>({1, 2, 3, 4, 5}));
    TestArchive(std::set<std::string>({"a", "bb", "ccc", "dddd"}));
    TestArchive(std::multiset<int>({1, 2, 3, 4, 5}));
    TestArchive(std::multiset<std::string>({"a", "bb", "ccc", "dddd"}));
    TestArchive(std::map<int, std::string>({std::make_pair(1, "a"), std::make_pair(2, "bb")}));
    TestArchive(std::multimap<int, std::string>({std::make_pair(1, "a"), std::make_pair(2, "bb")}));
}

TEST(Archive, POCOTest)
{
    // User type
    TestArchive(Message(666, "the beast"));
}

TEST(Archive, POCOToFileTest)
{
    int i = 123;
    std::string name = "mouton";
    float pi = 3.1416f;
    Message m(666, "the beast");

    // --- Serialize to file
    try
    {
        std::ofstream ofile("test.bin", std::ios::binary);
        Archive a(std::make_unique<OutputStream<std::ofstream>>(ofile));

        a << i << name << pi << m;
        ofile.close();
    }
    catch (std::exception const& e)
    {
        FAIL() << e.what() << " : " << strerror(errno);
    }

    int i2;
    std::string name2;
    float pi2;
    Message m2;

    // --- Unserialize from file:
    try
    {
        std::ifstream ifile("test.bin", std::ios::binary);

        if (!ifile.is_open())
        {
            std::cout << "ERROR file doesn't exist.." << std::endl;
            return;
        }

        Archive a2(std::make_unique<InputStream<std::ifstream>>(ifile));
        a2 >> i2;
        a2 >> name2;
        a2 >> pi2;
        a2 >> m2;
        ifile.close();
    }
    catch (std::exception const& e)
    {
        FAIL() << e.what() << " : " << strerror(errno);
    }

    // fk
    ASSERT_EQ(i2, i);
    ASSERT_EQ(name2, name);
    ASSERT_EQ(pi2, pi);
    ASSERT_TRUE(m2 == m); // Use builtin compoarison instead of memory address comparison
}

TEST(Archive, POCOMalformedDataTest)
{
    // invalid data, invalid string length
    std::stringstream s1;
    Archive a1(std::make_unique<Stream<std::stringstream>>(s1));

    a1 << std::string("salut");

    std::string data = s1.str();

    // Corrupt string length:
    data[0] = 255; // the first 4 bytes are the string length
    data[1] = 255; // the first 4 bytes are the string length
    data[2] = 255; // the first 4 bytes are the string length
    data[2] = 255; // the first 4 bytes are the string length

    // Try to read back the data:

    std::istringstream s2(data);
    Archive a2(std::make_unique<InputStream<std::istringstream>>(s2));

    std::string value;
    try
    {
        a2 >> value;
        FAIL() << "expected std::runtime_error";
    }
    catch (std::runtime_error const& e)
    {
        ASSERT_STREQ("malformed data", e.what());
    }
}


TEST(Archive, InvalidDataTest)
{
    // invalid data, not enough data
    std::stringstream s1;
    Archive a1(std::make_unique<Stream<std::stringstream>>(s1));
    //Archive<std::stringstream> a1(s1);
    a1 << 'a';

    std::string data = s1.str();

    // Try to read back the data:
    try
    {
        std::istringstream s2(data);
        Archive a2(std::make_unique<InputStream<std::istringstream>>(s2));

        int value;
        a2 >> value;
        FAIL() << "exception was not catched";
    }
    catch (std::runtime_error e)
    {
        SUCCEED();
    }
}

TEST(Archive, CharToStdSting)
{
    // given 
    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));

    //when
    a << "Hello World" << "";

    //then
    std::string str1, str2;
    a >> str1 >> str2;

    ASSERT_STREQ("Hello World", str1.c_str());
    ASSERT_TRUE(str2.empty());
}
