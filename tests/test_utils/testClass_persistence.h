#pragma once

#include "utils/persistence/persistence.h"
#include "utils/reference.h"

namespace ArchivePersistent
{
    /*
    */
    class EmptyClass : public Grafkit::Persistent, virtual public Referencable
    {
    public:
        EmptyClass() {
        }

        ~EmptyClass() {
        }

        PERSISTENT_DECL(EmptyClass, 1);
    protected:
        void Serialize(Grafkit::Archive& stream) override;;
    };

    /*
    */
    class FieldClass : public Grafkit::Persistent, virtual public Referencable
    {
    public:
        FieldClass();

        ~FieldClass() {
        }

        void Build(int i, float f);

        bool operator==(FieldClass& other) const { return m_int == other.m_int && m_float == other.m_float; }

    private:
        int m_int;
        float m_float;

        PERSISTENT_DECL(FieldClass, 1);
    protected:
        void Serialize(Grafkit::Archive& stream) override;;
    };


    /*
    */
    class NestedClass : public Grafkit::Persistent, virtual public Referencable
    {
    public:
        NestedClass();
        virtual ~NestedClass();

        void Buildup(int i1, float f1, int i2, float f2);

        bool operator==(NestedClass& other) const { return (*m_cl1) == (*other.m_cl1) && (*m_cl2) == (*other.m_cl2); }

    private:
        FieldClass* m_cl1;
        FieldClass* m_cl2;

        PERSISTENT_DECL(NestedClass, 1);

        void Serialize(Grafkit::Archive& stream) override;;
    };
}
