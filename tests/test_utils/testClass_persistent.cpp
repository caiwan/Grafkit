#include <cstring>
#include "testClass_persistent.h"

PERSISTENT_IMPL(ArchivePersistentTestClass)

ArchivePersistentTestClass::ArchivePersistentTestClass()
{
	memset(&this->m_data, 0, sizeof(this->m_data));
}

ArchivePersistentTestClass::ArchivePersistentTestClass(ArchiveBasicData_t & data)
{
	m_data = data;
}

ArchivePersistentTestClass::ArchivePersistentTestClass(ArchivePersistentTestClass & other)
{
	m_data = other.m_data;
}

void ArchivePersistentTestClass::serialize(Grafkit::Archive &stream)
{
	if (stream.isStoring()) {
		stream << m_data.m_int;
		stream << m_data.m_float;
	}
	else {
		stream >> m_data.m_int;
		stream >> m_data.m_float;
	}
}

// =========================================================================================== 

ArchivePersistentExtendedTestClass::ArchivePersistentExtendedTestClass()
{
}

ArchivePersistentExtendedTestClass::ArchivePersistentExtendedTestClass(ArchiveBasicData_t & data)
{
}

ArchivePersistentExtendedTestClass::ArchivePersistentExtendedTestClass(ArchivePersistentExtendedTestClass & other)
{
}

void ArchivePersistentExtendedTestClass::serialize(Grafkit::Archive & stream)
{
}

ArchivePersistentLongClass::ArchivePersistentLongClass()
{
}

ArchivePersistentLongClass::ArchivePersistentLongClass(ArchiveBasicData_t & data)
{
}

ArchivePersistentLongClass::ArchivePersistentLongClass(ArchivePersistentLongClass & other)
{
}

void ArchivePersistentLongClass::serialize(Grafkit::Archive & stream)
{
}
