#include "stdafx.h"
#include <cstring>
//#include "testClass_persistence.h"

# if 0

using namespace ArchivePersistent;

PERSISTENT_IMPL(EmptyClass);
PERSISTENT_IMPL(FieldClass);
PERSISTENT_IMPL(NestedClass);

/*
*/

void EmptyClass::Serialize(Grafkit::Archive& stream) {
}

/*
*/

FieldClass::FieldClass(): m_int(0)
    , m_float(0) {
}

void FieldClass::Build(int i, float f)
{
    m_int = i;
    m_float = f;
}

void FieldClass::Serialize(Grafkit::Archive& stream)
{
    PERSIST_FIELD(stream, m_int);
    PERSIST_FIELD(stream, m_float);
}

/*
*/

NestedClass::NestedClass(): m_cl1(nullptr)
    , m_cl2(nullptr) {
}

NestedClass::~NestedClass()
{
    delete m_cl1;
    delete m_cl2;
}

void NestedClass::Buildup(int i1, float f1, int i2, float f2)
{
    m_cl1 = new FieldClass();
    m_cl1->Build(i1, f1);

    m_cl2 = new FieldClass();
    m_cl2->Build(i2, f2);
}

void NestedClass::Serialize(Grafkit::Archive& stream)
{
    PERSIST_OBJECT(stream, m_cl1);
    PERSIST_OBJECT(stream, m_cl2);
}

#endif