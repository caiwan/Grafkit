#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/persistence/dynamics.h"
#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
//#include "testClass_persistent.h"


using namespace Grafkit;
using namespace FWdebugExceptions;

TEST(Persistent, Persistence_Field) {
	TestArchiver archive(18, true);
	
	int test = 0xfacababa;
	archive & new PersistField<int>("test", &test);

	archive.hexdump();
}

//TEST(Persistent, Persistence_Field) {
//
//}

//
//// due packing issues and alignment
//#pragma pack(push, 1)
//#include "persistent_test_data_0.inc"
//
//// basic load tests
//TEST(Persistent, Load)
//{
//	const td0_t *in_data = ((td0_t*)test_data_0);
//	TestArchiver archive(in_data, size_test_data_0);
//
//	Persistent* object = Persistent::load(archive);
//
//	ASSERT_FALSE(NULL == object);
//	ASSERT_FALSE(NULL == dynamic_cast<ArchivePersistentTestClass*>(object));
//
//	ArchivePersistentTestClass* testobj = dynamic_cast<ArchivePersistentTestClass*>(object);
//
//	ASSERT_EQ(testobj->getData().m_int, 0xfacababa);
//	ASSERT_TRUE(testobj->getData().m_float - 2.16 < 0.00001);
//
//}
//
//TEST(Persistent, Load_Package_VersionMismatch)
//{
//	td0_t in_data = *((td0_t*)test_data_0);
//
//	in_data.header.major += 3;
//	in_data.header.minor += 3;
//
//	ASSERT_THROW(TestArchiver archive(&in_data, size_test_data_0), PersistentVersionMismatch);
//}
//
//TEST(Persistent, Load_Class_VersionMismatch)
//{
//	td0_t in_data = *((td0_t*)test_data_0);
//
//	in_data.clazz.classversion += 3;
//	
//	TestArchiver archive(&in_data, size_test_data_0);
//	ASSERT_THROW(Persistent* object = Persistent::load(archive), PersistentVersionMismatch);
//}
//
//TEST(Persistent, Load_Class_NameMismatch)
//{
//	td0_t in_data = *((td0_t*)test_data_0);
//
//	strcpy_s(in_data.clazz.classname, "__nonexistent_data__");
//
//	TestArchiver archive(&in_data, size_test_data_0);
//	ASSERT_THROW(Persistent* object = Persistent::load(archive), PersistentCreateObjectExcpetion);
//}
//
///**
//	Test save with float and int members
//*/
//TEST(Persistent, Save_Basic) {
//	TestArchiver archive_save(256, true);
//	archive_save.setDirection(true);
//
//	ArchiveBasicData_t test_data;
//	test_data.m_float = 2.16f;
//	test_data.m_int = 0xfacababa;
//
//	// store data
//	ArchivePersistentTestClass dynamics(test_data);
//	dynamics.store(archive_save);
//
//	// load data back
//	TestArchiver archive_load(archive_save.getBuffer(), archive_save.getSize());
//
//	Persistent *obj = Persistent::load(archive_load);
//	ASSERT_FALSE(obj == NULL);
//	
//	ArchivePersistentTestClass* testobj = dynamic_cast<ArchivePersistentTestClass*>(obj);
//	ASSERT_FALSE(NULL == testobj);
//
//	// check contents
//	ASSERT_EQ(testobj->getData().m_int, test_data.m_int);
//	ASSERT_TRUE(testobj->getData().m_float - test_data.m_float < 0.00001);
//}
//
///**
//	Test save and load with all basic types
//*/
//TEST(Persistent, Save_Extended) {
//	TestArchiver archive_save(256, true);
//	archive_save.setDirection(true);
//
//	ArchiveExtendedData_t test_data; 
//	
//	test_data.m_float = 2.16f;
//	test_data.m_int = -65537;
//
//	test_data.m_char = 'c';
//	test_data.m_uchar = 127; 
//	test_data.m_short = -512;
//	test_data.m_ushort = 0xeffe; 
//	test_data.m_uint = 0xfacababa;
//	test_data.m_double = 3.14159265359;
//
//	// store data
//	ArchivePersistentExtendedTestClass dynamics(test_data);
//	dynamics.store(archive_save);
//
//	// load data back
//	TestArchiver archive_load(archive_save.getBuffer(), archive_save.getSize());
//
//	Persistent *obj = Persistent::load(archive_load);
//	ASSERT_FALSE(obj == NULL);
//
//	ArchivePersistentTestClass* testobj = dynamic_cast<ArchivePersistentTestClass*>(obj);
//	ASSERT_FALSE(NULL == testobj);
//
//	// check contents
//	FAIL();
//
//	//ASSERT_EQ(testobj->getData().m_int, test_data.m_int);
//	//ASSERT_TRUE(testobj->getData().m_float - test_data.m_float < 0.00001);
//}
//
//TEST(Persistent, Save_LongTypes) {
//	TestArchiver archive_save(256, true);
//	archive_save.setDirection(true);
//
//	ArchiveLongData_t test_data;
//
//	test_data.m_float = 2.16f;
//	test_data.m_int = -65537;
//
//	test_data.m_char = 'c';
//	test_data.m_uchar = 127;
//	test_data.m_short = -512;
//	test_data.m_ushort = 0xeffe;
//	test_data.m_uint = 0xfacababa;
//	test_data.m_double = 3.14159265359;
//
//	test_data.m_long = 0xfefefefefefefefe;
//	test_data.m_ulong = 0xfefefefefefefefe;
//	test_data.m_dlong = 0xfefefefefefefefe;
//	test_data.m_udlong = 0xfefefefefefefefe;
//	test_data.m_longdouble = 3.14159265358979323846264338327950288419716939937510;
//
//	// store data
//	ArchivePersistentExtendedTestClass dynamics(test_data);
//	dynamics.store(archive_save);
//
//	// load data back
//	TestArchiver archive_load(archive_save.getBuffer(), archive_save.getSize());
//
//	Persistent *obj = Persistent::load(archive_load);
//	ASSERT_FALSE(obj == NULL);
//
//	ArchivePersistentTestClass* testobj = dynamic_cast<ArchivePersistentTestClass*>(obj);
//	ASSERT_FALSE(NULL == testobj);
//
//	// check contents
//	FAIL();
//
//	//ASSERT_EQ(testobj->getData().m_int, test_data.m_int);
//	//ASSERT_TRUE(testobj->getData().m_float - test_data.m_float < 0.00001);
//}
//
//
//
//#pragma pack(pop)
