#pragma once

// #include <vector>

#include "../exceptions.h"
#include "dynamics.h"
#include "tlv_format.h"

// http://www.codeproject.com/Tips/495191/Serialization-implementation-in-Cplusplus

//#define NOT_USING_RTTI

#define PERSISTENT_SIGNATURE "CAI"
#define PERSISTENT_MAJOR_VER 0
#define PERSISTENT_MINOR_VER 2

namespace Grafkit{
	class Archive;

	class Persistent : public Clonable
	{
		public:
			virtual ~Persistent() {}
			static Persistent* load(Archive& stream);
			void store(Archive& stream) const;

		protected:
			virtual void serialize(Archive& stream);

			///@todo use typeid(T).name() instead, and do the lookup by that;
			virtual const char* getClassName() const = 0;
			virtual int version() const { return 0; }
	};

	class PersistElem;
	template <typename T> class PersistField;
	
	class PersistElem {
		friend class Archive;
	protected:
		virtual void load(Archive &ar) = 0;
		virtual void store(Archive & ar) = 0;
	};

	class Archive
	{
		friend class PersistElem;
		template <typename T> friend class PersistField;

		public:
			explicit Archive(int isStoring = 0);
			virtual ~Archive();

		protected:
			/// I/O stuff
			virtual void write(const void* buffer, size_t length) = 0;
			virtual void read(void* buffer, size_t length) = 0;
	
		public:

			Archive& operator& (PersistElem *field) {
				if (isStoring()) {
					field->store(*this);
				}
				else {
					field->load(*this);
				}

				delete field;

				return *this;
			}

			int isStoring() const { return _isStoring; }
			void setDirection(bool isStoring) { _isStoring = isStoring; }

		private: 
			int _isStoring; // fix'd
			short _major;
			short _minor;
	};

	template <typename T> class PersistField : public PersistElem {

	private:
		T* m_pT;

	public:
		PersistField(const char* name, T* ptr) {
			this->m_pT = ptr;
		}

		~PersistField() {
		}

	protected:
		virtual void load(Archive &ar) {
			ar.read(m_pT, sizeof(T));
		}
		
		virtual void store(Archive & ar) {
			ar.write(m_pT, sizeof(T));
		}

	};

	template <typename T> class PersistVector : public PersistElem {
		friend class Archive;

	private:
		unsigned int m_count;
		T** m_pT;

	public:
		PersistVector(const char* name, T** ptr, unsigned int * pcount) {
			this->m_pT = ptr;
			this->m_count = pcount;
		}

		~PersistVector() {
		}

	protected:
		virtual void load(Archive &ar) {
			unsigned int count = 0;
			ar.read(m_count, sizeof(m_count[0]));
			ar.read(*m_pT, sizeof(T) * (*m_count));
		}

		virtual void store(Archive & ar) {
			ar.write(m_count, sizeof(m_count[0]));
			ar.write(*m_pT, sizeof(T) * (*m_count));
		}

	};
}


// ------------------

#define PERSISTENT_DECL(className, VERSION_NO) \
public: \
	CLONEABLE_DECL(className)\
	CLONEABLE_FACTORY_DECL(className)\
public:\
	virtual const char* getClassName() const\
	{ \
		return #className; \
	} \
	virtual int version() const \
	{ \
		return VERSION_NO;\
	}\
private: \
	static Grafkit::AddClonable _addClonable;

#define PERSISTENT_IMPL(className) \
	CLONEABLE_FACTORY_IMPL(className)

// --- 

#define PERSIST_FIELD(FIELD)
#define PERSIST_VECTOR(FIELD, COUNT)


// --- define exceptions 

DEFINE_EXCEPTION(PersistentCreateObjectExcpetion, 0, "Load error: Cannot create object")
DEFINE_EXCEPTION(PersistentVersionMismatch, 0, "Load error: Version mismatch")
