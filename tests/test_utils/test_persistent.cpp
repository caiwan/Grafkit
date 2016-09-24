#define _CRT_SECURE_NO_WARNINGS

#include <gtest/gtest.h>

#include "utils/dynamics.h"
#include "utils/persistent.h"

#include "TestArchiver.h"
#include "testClass_persistent.h"

// for debug purposes
// defining this macro allows us to write the objct to a file as well

// #define WRITER

#ifdef WRITER
#include "archive.h"
#include <cstdio>
#include <cstdlib>
#endif

using namespace Grafkit;
using namespace FWdebugExceptions;

#define PERSISTENT_classname "ArchivePersistentTestClass"
#define PERSISTENT_classname_ex "ArchivePersistentExtendedTestClass"
#define PERSISTENT_classname_long "ArchivePersistentLongClass"

TEST(Persistent, DataStructure) {
	TestArchiver archive(256, true);
	archive.setDirection(true);

	ArchiveBasicData_t test_data;
	test_data.m_float = 2.16f;
	test_data.m_int = 0xfacababa;

	ArchivePersistentTestClass dynamics(test_data);

	dynamics.store(archive);
	
#ifdef WRITER
	FILE* fp = fopen("out.bin_", "wb");
	if (fp)
	{
		ArchiveFile fout(fp, true);
		dynamics.store(fout);
		fflush(fp);
		fclose(fp);
	}
#endif

	unsigned char* data = archive.getBuffer();
	size_t len = archive.getSize();
	size_t crsr = archive.getCrsr();

	ASSERT_EQ(55, crsr);

	// --- header
	unsigned int offset = 0;
	unsigned int header_str_len = *((unsigned int *)&data[offset + 0]);
	char *header_str = ((char *)&data[offset + 4]); offset += 4 + header_str_len + 1;

	unsigned int header_maj_ver = *((unsigned short *)&data[offset + 0]);
	unsigned int header_min_ver = *((unsigned short *)&data[offset + 2]); offset += 4;

	ASSERT_EQ(strlen(PERSISTENT_SIGNATURE), header_str_len);
	ASSERT_TRUE(0 == memcmp(header_str, PERSISTENT_SIGNATURE, header_str_len));

	ASSERT_EQ(PERSISTENT_MAJOR_VER, header_maj_ver);
	ASSERT_EQ(PERSISTENT_MINOR_VER, header_min_ver);

	// --- content: class header
	unsigned int class_header_str_len = *((unsigned int *)&data[offset + 0]);
	char *class_header_str = ((char *)&data[offset + 4]); offset += 4 + class_header_str_len + 1;

	unsigned int class_header_version = *((unsigned int *)&data[offset + 0]); offset += 4;

	ASSERT_EQ(strlen(PERSISTENT_classname), class_header_str_len);
	ASSERT_TRUE(0 == memcmp(class_header_str, "ArchivePersistentTestClass", class_header_str_len));

	// -- check factory 
	const Clonable *factory = Clonables::Instance().find(class_header_str);
	ASSERT_FALSE(NULL == factory);

	Clonable *instance = factory->createObj();
	ASSERT_FALSE(NULL == instance);
	delete instance;


	// -- content: class content
	unsigned int class_integer = *((unsigned int *)&data[offset + 0]);
	float class_float = *((float *)&data[offset + 4]);

	ASSERT_EQ(0xFACABABA, class_integer);
	ASSERT_TRUE(abs(class_float-2.16) < 0.00001);
}

// due packing issues and alignment
#pragma pack(push, 1)
#include "persistent_test_data_0.inc"

// basic load tests
TEST(Persistent, Load)
{
	const td0_t *in_data = ((td0_t*)test_data_0);
	TestArchiver archive(in_data, size_test_data_0);

	Persistent* object = Persistent::load(archive);

	ASSERT_FALSE(NULL == object);
	ASSERT_FALSE(NULL == dynamic_cast<ArchivePersistentTestClass*>(object));

	ArchivePersistentTestClass* testobj = dynamic_cast<ArchivePersistentTestClass*>(object);

	ASSERT_EQ(testobj->getData().m_int, 0xfacababa);
	ASSERT_TRUE(testobj->getData().m_float - 2.16 < 0.00001);

}

TEST(Persistent, Load_Package_VersionMismatch)
{
	td0_t in_data = *((td0_t*)test_data_0);

	in_data.header.major += 3;
	in_data.header.minor += 3;

	ASSERT_THROW(TestArchiver archive(&in_data, size_test_data_0), PersistentVersionMismatch);
}

TEST(Persistent, Load_Class_VersionMismatch)
{
	td0_t in_data = *((td0_t*)test_data_0);

	in_data.clazz.classversion += 3;
	
	TestArchiver archive(&in_data, size_test_data_0);
	ASSERT_THROW(Persistent* object = Persistent::load(archive), PersistentVersionMismatch);
}

TEST(Persistent, Load_Class_NameMismatch)
{
	td0_t in_data = *((td0_t*)test_data_0);

	strcpy(in_data.clazz.classname, "__nonexistent_data__");

	TestArchiver archive(&in_data, size_test_data_0);
	ASSERT_THROW(Persistent* object = Persistent::load(archive), PersistentCreateObjectExcpetion);
}

/**
	Test save with float and int members
*/
TEST(Persistent, Save_Basic) {
	TestArchiver archive_save(256, true);
	archive_save.setDirection(true);

	ArchiveBasicData_t test_data;
	test_data.m_float = 2.16f;
	test_data.m_int = 0xfacababa;

	// store data
	ArchivePersistentTestClass dynamics(test_data);
	dynamics.store(archive_save);

	// load data back
	TestArchiver archive_load(archive_save.getBuffer(), archive_save.getSize());

	Persistent *obj = Persistent::load(archive_load);
	ASSERT_FALSE(obj == NULL);
	
	ArchivePersistentTestClass* testobj = dynamic_cast<ArchivePersistentTestClass*>(obj);
	ASSERT_FALSE(NULL == testobj);

	// check contents
	ASSERT_EQ(testobj->getData().m_int, test_data.m_int);
	ASSERT_TRUE(testobj->getData().m_float - test_data.m_float < 0.00001);
}

/**
	Test save and load with all basic types
*/
TEST(Persistent, Save_Extended) {
	TestArchiver archive_save(256, true);
	archive_save.setDirection(true);

	ArchiveExtendedData_t test_data; 
	
	test_data.m_float = 2.16f;
	test_data.m_int = -65537;

	test_data.m_char = 'c';
	test_data.m_uchar = 127; 
	test_data.m_short = -512;
	test_data.m_ushort = 0xeffe; 
	test_data.m_uint = 0xfacababa;
	test_data.m_double = 3.14159265359;

	// store data
	ArchivePersistentExtendedTestClass dynamics(test_data);
	dynamics.store(archive_save);

	// load data back
	TestArchiver archive_load(archive_save.getBuffer(), archive_save.getSize());

	Persistent *obj = Persistent::load(archive_load);
	ASSERT_FALSE(obj == NULL);

	ArchivePersistentTestClass* testobj = dynamic_cast<ArchivePersistentTestClass*>(obj);
	ASSERT_FALSE(NULL == testobj);

	// check contents
	FAIL();

	//ASSERT_EQ(testobj->getData().m_int, test_data.m_int);
	//ASSERT_TRUE(testobj->getData().m_float - test_data.m_float < 0.00001);
}

TEST(Persistent, Save_LongTypes) {
	TestArchiver archive_save(256, true);
	archive_save.setDirection(true);

	ArchiveLongData_t test_data;

	test_data.m_float = 2.16f;
	test_data.m_int = -65537;

	test_data.m_char = 'c';
	test_data.m_uchar = 127;
	test_data.m_short = -512;
	test_data.m_ushort = 0xeffe;
	test_data.m_uint = 0xfacababa;
	test_data.m_double = 3.14159265359;

	test_data.m_long = 0xfefefefefefefefe;
	test_data.m_ulong = 0xfefefefefefefefe;
	test_data.m_dlong = 0xfefefefefefefefe;
	test_data.m_udlong = 0xfefefefefefefefe;
	test_data.m_longdouble = 3.14159265358979323846264338327950288419716939937510;

	// store data
	ArchivePersistentExtendedTestClass dynamics(test_data);
	dynamics.store(archive_save);

	// load data back
	TestArchiver archive_load(archive_save.getBuffer(), archive_save.getSize());

	Persistent *obj = Persistent::load(archive_load);
	ASSERT_FALSE(obj == NULL);

	ArchivePersistentTestClass* testobj = dynamic_cast<ArchivePersistentTestClass*>(obj);
	ASSERT_FALSE(NULL == testobj);

	// check contents
	FAIL();

	//ASSERT_EQ(testobj->getData().m_int, test_data.m_int);
	//ASSERT_TRUE(testobj->getData().m_float - test_data.m_float < 0.00001);
}



#pragma pack(pop)
