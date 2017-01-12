#pragma once

#include "../exceptions.h"
#include "dynamics.h"

// http://www.codeproject.com/Tips/495191/Serialization-implementation-in-Cplusplus

namespace Grafkit{
	class Archive;

	class Persistent : public Clonable
	{
		friend class PersistObject;
		public:
			static Persistent* load(Archive& ar);
			void store(Archive& ar);

		protected:
			virtual void serialize(Archive& ar) = 0;

			///@todo use typeid(T).name() instead, and do the lookup by that;
			virtual std::string getClassName() const = 0;
			virtual int version() const { return 0; }
	};

	class Archive
	{
		public:
			explicit Archive(int IsStoring = 0);
			virtual ~Archive();

			/* IO API */
		protected:
			virtual void Write(const void* buffer, size_t length) = 0;
			virtual void Read(void* buffer, size_t length) = 0;
	
			size_t WriteString(const char* input); ///< @return length of written bytes of string
			size_t ReadString(char*& output);	///< @return length of read bytes of string

		public:

			template <typename T> void PersistField(T& t) {
				if (m_isStoring) {
					Write(&t, sizeof(T));
				}
				else {
					Read(&t, sizeof(T));
				}
			}

			template <typename T> void PersistVector(T *& v, size_t &count) {
				UINT u32count = count; //clamp it down to 32 bit
				if (m_isStoring) {
					Write(&u32count, sizeof(u32count))
					Write(&t, sizeof(T));
				}
				else {
					u32count = 0;
					Read(&u32count, sizeof(u32count));

					v = new T[u32count];
					Read(&v, sizeof(T) * u32count);
					count = u32count;
				}
			}
			

			void PersistString(const char*& str);
			void PersistString(std::string & str);

			void PersistObject(Persistent *& object);

			int IsStoring() const { return m_isStoring; }
			void SetDirection(bool IsStoring) { m_isStoring = IsStoring; }

		private: 
			int m_isStoring;
	};


}


// ------------------

#define PERSISTENT_DECL(className, VERSION_NO) \
public: \
	CLONEABLE_FACTORY_DECL(className)\
public:\
	virtual std::string getClassName() const\
	{ \
		return #className; \
	} \
	virtual int version() const \
	{ \
		return VERSION_NO;\
	}

#define PERSISTENT_IMPL(className) \
	CLONEABLE_FACTORY_IMPL(className)

// --- 

#define PERSIST_FIELD(AR, FIELD) (AR.PersistField<decltype((FIELD))>((FIELD)))
#define PERSIST_VECTOR(AR, FIELD, COUNT) (AR.PersistVector<std::remove_pointer<decltype((FIELD))>::type>((FIELD), (COUNT)))
#define PERSIST_STRING(AR, FIELD) (AR.PersistString((FIELD)))
#define PERSIST_OBJECT(AR, FIELD) (AR.PersistObject((FIELD)))


// --- define exceptions 

DEFINE_EXCEPTION(PersistentCreateObjectExcpetion, 0, "Load error: Cannot create object")
DEFINE_EXCEPTION(PersistentVersionMismatch, 0, "Load error: Version mismatch")
