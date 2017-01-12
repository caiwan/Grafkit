#pragma once

#include "utils/persistence/persistence.h"
#include "utils/reference.h"

namespace ArchivePersistent {
	/*
	*/
	class EmptyClass : public Grafkit::Persistent, virtual public Referencable {

	public:
		EmptyClass() {}
		~EmptyClass() {}

	protected:
		PERSISTENT_DECL(EmptyClass, 1);
		virtual void serialize(Grafkit::Archive& stream) {};
	};

	/*
	*/
	class FieldClass : public Grafkit::Persistent, virtual public Referencable {

	public:
		FieldClass() {}
		~FieldClass() {}

		void buildup(int i, float f) {
			m_int = i;
			m_float = f;
		}

		bool operator==(FieldClass &other) {
			return m_int == other.m_int && m_float == other.m_float;
		}

	private:
		int m_int;
		float m_float;

		PERSISTENT_DECL(FieldClass, 1);
	protected:
		virtual void serialize(Grafkit::Archive& stream) {
			PERSIST_FIELD(steam, m_int);
			PERSIST_FIELD(steam, m_float);
		};

	};


	/*
	*/
	class NestedClass : public Grafkit::Persistent, virtual public Referencable {
	public:
		NestedClass() : m_cl1(nullptr), m_cl2(nullptr) {}
		~NestedClass() {
			delete m_cl1;
			delete m_cl2;
		}

		void buildup(int i1, float f1, int i2, float f2) {
			m_cl1 = new FieldClass();
			m_cl1->buildup(i1, f1);

			m_cl2 = new FieldClass();
			m_cl2->buildup(i2, f2);
		}

		bool operator==(NestedClass &other) {
			return (*m_cl1) == (*other.m_cl1) && (*m_cl2) == (*other.m_cl2);
		}

	private:
		FieldClass *m_cl1;
		FieldClass *m_cl2;

		PERSISTENT_DECL(NestedClass, 1);

		virtual void serialize(Grafkit::Archive& stream) {
			PERSIST_OBJECT(stream, m_cl1);
			PERSIST_OBJECT(stream, m_cl2);
		};

	};


#if 0
	/*
	*/
	class NestedClassRef : public Grafkit::Persistent, virtual public Referencable {
		PERSISTENT_DECL(EmptyClass, 1);

		virtual void serialize(Grafkit::Archive& stream) {

		};

	public:
		NestedClassRef() : m_rcl1(nullptr), m_rcl2(nullptr) {}
		~NestedClassRef() {}

	private:
		Ref<FieldClass> m_rcl1;
		Ref<FieldClass> m_rcl2;

	};

	class ComplexClass() : public Grafkit::Persistent {
		// ...
	}

#endif

}