#pragma once

#include "utils/persistence/persistence.h"
#include "utils/reference.h"

/*
*/
class ArchivePersistentTestEmptyClass : public Grafkit::Persistent, virtual public Referencable {

public:
	ArchivePersistentTestEmptyClass() {}
	~ArchivePersistentTestEmptyClass() {}

protected:
	PERSISTENT_DECL(ArchivePersistentTestEmptyClass, 1);
	virtual void serialize(Grafkit::Archive& stream) {};
};

/*
*/
class ArchivePersistentTestFieldClass : public Grafkit::Persistent, virtual public Referencable {

public:
	ArchivePersistentTestFieldClass(int i = 0xfacababa, float f = 2.16) : m_int(i), m_float(f){}
	~ArchivePersistentTestFieldClass() {}

	bool operator==(ArchivePersistentTestFieldClass &other) {
		return (
			m_int == other.m_int
			&& m_float == other.m_float
			);
	}

private:
	int m_int;
	float m_float;

	PERSISTENT_DECL(ArchivePersistentTestFieldClass, 1);
protected:
	virtual void serialize(Grafkit::Archive& stream) {
		PERSIST_FIELD(m_int);
		PERSIST_FIELD(m_float);
	};

};

#if 0
/*
*/
class ArchivePersistentTestNestedClass : public Grafkit::Persistent, virtual public Referencable {
	PERSISTENT_DECL(ArchivePersistentTestEmptyClass, 1);

	virtual void serialize(Grafkit::Archive& stream) {
		
	};

public:
	ArchivePersistentTestNestedClass() {}
	~ArchivePersistentTestNestedClass() {}

private:
	ArchivePersistentTestFieldClass m_cl1;
	ArchivePersistentTestFieldClass m_cl2;

};

/*
*/
class ArchivePersistentTestNestedClassPtr : public Grafkit::Persistent {
	PERSISTENT_DECL(ArchivePersistentTestEmptyClass, 1);

	virtual void serialize(Grafkit::Archive& stream) {

	};

public:
	ArchivePersistentTestNestedClassPtr() : m_pcl1 (nullptr), m_pcl2(nullptr){}
	~ArchivePersistentTestNestedClassPtr() {}

private:
	ArchivePersistentTestFieldClass *m_pcl1;
	ArchivePersistentTestFieldClass *m_pcl2;

};

/*
*/
class ArchivePersistentTestNestedClassRef : public Grafkit::Persistent, virtual public Referencable {
	PERSISTENT_DECL(ArchivePersistentTestEmptyClass, 1);

	virtual void serialize(Grafkit::Archive& stream) {

	};

public:
	ArchivePersistentTestNestedClassRef() : m_rcl1(nullptr), m_rcl2(nullptr) {}
	~ArchivePersistentTestNestedClassRef() {}

private:
	Ref<ArchivePersistentTestFieldClass> m_rcl1;
	Ref<ArchivePersistentTestFieldClass> m_rcl2;

};
#endif
