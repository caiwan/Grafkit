#pragma once

#include "../exceptions.h"
#include "dynamics.h"

// http://www.codeproject.com/Tips/495191/Serialization-implementation-in-Cplusplus

namespace Grafkit{
	class Archive;

	class PersistElem;
	template <typename T> class PersistField;
	template <typename T> class PersistVector;
	class PersistString;
	template <typename K, typename V> class PersistMap;
	class PersistObject;

	class Persistent : public Clonable
	{
		friend class PersistObject;
		public:
			virtual ~Persistent() {}
			
			static Persistent* load(Archive& ar);
			void store(Archive& ar);

		protected:
			virtual void serialize(Archive& ar) = 0;

			///@todo use typeid(T).name() instead, and do the lookup by that;
			virtual const char* getClassName() const = 0;
			virtual int version() const { return 0; }
	};

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
		template <typename T> friend class PersistVector;
		friend class PersistString;
		template <typename K, typename V> friend class PersistMap;
		friend class PersistObject;


		public:
			explicit Archive(int isStoring = 0);
			virtual ~Archive();

		protected:
			/// I/O stuff
			virtual void write(const void* buffer, size_t length) = 0;
			virtual void read(void* buffer, size_t length) = 0;
	
			size_t writeString(const char* input); ///< @return length of written bytes of string
			size_t readString(char*& output);	///< @return length of read bytes of string

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

	/** */
	template <typename T> class PersistField : public PersistElem {

	private:
		T& m_pT;

	public:
		PersistField(const char* name, T& t) : m_pT(t){
		}

		~PersistField() {
		}

	protected:
		virtual void load(Archive &ar) {
			ar.read(&m_pT, sizeof(T));
		}
		
		virtual void store(Archive & ar) {
			ar.write(&m_pT, sizeof(T));
		}

	};

	/** */
	template <typename T> class PersistVector : public PersistElem {
		friend class Archive;

	private:
		size_t & m_count;
		T*& m_pT;

	public:
		/// Only works with size_t from now on
		PersistVector(const char* name, T*& pt, size_t & len) :  
			m_pT(pt), m_count(len)
		{
		}

		~PersistVector() {
		}

	protected:
		virtual void load(Archive &ar) {
			unsigned int count = 0;
			ar.read(&count, sizeof(count));
			m_pT = new T[count];
			ar.read((void*)m_pT, sizeof(T) * (count));
			m_count = count;
		}

		virtual void store(Archive & ar) {
			unsigned int count = m_count;
			ar.write(&count, sizeof(count));
			ar.write(m_pT, sizeof(T) * count);
		}

	};

	/** */
	class PersistString : public PersistElem {
	private:
		std::string *m_pString;
		const char **m_pszString;

	public:
		PersistString(const char * name, const char **pszString) {
			m_pszString = pszString;
			m_pString = nullptr;
		}

		PersistString(const char * name, std::string *pString) {
			m_pszString = nullptr;
			m_pString = pString;
		}

		virtual void load(Archive &ar);
		virtual void store(Archive & ar);
	};

	/** */
	class PersistObject : public PersistElem {
	
	private:
		Persistent **m_pObject;
	
	public:
		PersistObject(const char* name, Persistent** pObject) : m_pObject(pObject){}
		
		virtual void load(Archive &ar) {
			unsigned char isNull = 0;
			ar & (new PersistField<unsigned char>("null", isNull));
			if (isNull)
				*m_pObject = nullptr;
			else
				*m_pObject = Persistent::load(ar);
		}
		virtual void store(Archive & ar){
			unsigned char isNull = (*m_pObject) == nullptr;
			ar & (new PersistField<unsigned char>("null", isNull));
			if (!isNull)
				(*m_pObject)->store(ar);
		}
	};

}


// ------------------

#define PERSISTENT_DECL(className, VERSION_NO) \
public: \
	CLONEABLE_FACTORY_DECL(className)\
public:\
	virtual const char* getClassName() const\
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

#define PERSIST_FIELD(FIELD) (new Grafkit::PersistField<decltype(FIELD)>(#FIELD, FIELD))
#define PERSIST_VECTOR(FIELD, COUNT) (new Grafkit::PersistVector<std::remove_pointer<decltype(FIELD)>::type>(#FIELD, FIELD, COUNT))
#define PERSIST_STRING(FIELD) (new Grafkit::PersistString(#FIELD, FIELD))
#define PERSIST_OBJECT(FIELD) (new Grafkit::PersistObject(#FIELD, (Persistent**)&(FIELD)))


// --- define exceptions 

DEFINE_EXCEPTION(PersistentCreateObjectExcpetion, 0, "Load error: Cannot create object")
DEFINE_EXCEPTION(PersistentVersionMismatch, 0, "Load error: Version mismatch")
