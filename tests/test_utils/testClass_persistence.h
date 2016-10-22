#pragma once

#include "utils/persistence/persistence.h"

struct ArchiveBasicData_t
{
	int m_int;
	float m_float;
};

struct ArchiveExtendedData_t : public ArchiveBasicData_t
{
	char m_char;
	unsigned char m_uchar;
	short m_short;
	unsigned short m_ushort;
	unsigned int m_uint;
	double m_double;
};

struct ArchiveLongData_t : public ArchiveExtendedData_t
{
	long m_long;
	unsigned long m_ulong;
	long long m_dlong;
	unsigned long long m_udlong;
	long double m_longdouble;

};

#define PCLASS_VERSION 1

/** Base test for persistent w/ an int and float member
*/
class ArchivePersistentTestClass : public Grafkit::Persistent {
public:
	ArchivePersistentTestClass();
	ArchivePersistentTestClass(ArchiveBasicData_t& data);
	ArchivePersistentTestClass(ArchivePersistentTestClass& other);

	virtual void serialize(Grafkit::Archive& stream);

	ArchiveBasicData_t &getData() { return this->m_data; }

private:
	ArchiveBasicData_t m_data;
	PERSISTENT_DECL(ArchivePersistentTestClass, PCLASS_VERSION);

};

/** A base class to test out every desired base types
*/
class ArchivePersistentExtendedTestClass : public Grafkit::Persistent {
public:
	ArchivePersistentExtendedTestClass();
	ArchivePersistentExtendedTestClass(ArchiveBasicData_t& data);
	ArchivePersistentExtendedTestClass(ArchivePersistentExtendedTestClass& other);

	virtual void serialize(Grafkit::Archive& stream);

private:
	ArchiveBasicData_t m_data;
	PERSISTENT_DECL(ArchivePersistentExtendedTestClass, PCLASS_VERSION);
};

/** 
*/
class ArchivePersistentLongClass : public Grafkit::Persistent {
public:
	ArchivePersistentLongClass();
	ArchivePersistentLongClass(ArchiveBasicData_t& data);
	ArchivePersistentLongClass(ArchivePersistentLongClass& other);

	virtual void serialize(Grafkit::Archive& stream);

private:
	ArchiveBasicData_t m_data;
	PERSISTENT_DECL(ArchivePersistentExtendedTestClass, PCLASS_VERSION);
};