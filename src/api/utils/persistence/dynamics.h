#ifndef __DYNAMICS_H__
#define __DYNAMICS_H__

#include <map>
#include <string>

#ifdef DEBUG
#include "../logger.h"
#endif

/// http://www.codeproject.com/Tips/495191/Serialization-implementation-in-Cplusplus

namespace Grafkit {
	class Clonables;

	/**
	*/
	class Clonable
	{
		friend class Clonables;
	public:
		virtual Clonable* createObj() const = 0;
	};

	/**
	*/
	class Clonables {
	private:
		std::map<std::string, const Clonable*> m_clonables;

	private:
		Clonables() {}

		Clonables(const Clonables&) = delete;
		Clonables& operator=(const Clonables&) = delete;

		virtual ~Clonables()
		{
			for (auto it = m_clonables.begin(); it != m_clonables.end(); ++it) {
				const Clonable* clone = it->second;
				delete clone;
			}
			m_clonables.clear();
		}

	public:
		static Clonables& Instance()
		{
			static Clonables instance;   // Guaranteed to be destroyed.                              
			return instance;    // Instantiated on first use.
		}

	public:
		void addClonable(const char* className, Clonable* clone)
		{
			std::string name = className;

			auto it = m_clonables.find(name);
			if (it == m_clonables.end()) {
				m_clonables[name] = clone;
			}
		}

		const Clonable* find(const char *className)
		{
			std::string name = className;
			auto it = m_clonables.find(name);
			if (it == m_clonables.end())
				return NULL;
			const Clonable* clone = it->second;
			return clone;
		}

		Clonable* create(const char *className)
		{
			const Clonable* clone = find(className);
			if (clone)
				return clone->createObj();
			else
				return NULL;
		}

#ifdef DEBUG
		void dumpClonables() {
			for (auto it = m_clonables.begin(); it != m_clonables.end(); ++it) {
				Log::Logger().Debug("- Has Clonable factory of %s", it->first.c_str());
			}
		}
#endif //DEBUG
	};

	/**
	*/
	class AddClonable {
	public:
		AddClonable(const char* className, Clonable* clone) {
			Clonables::Instance().addClonable(className, clone);
		}
	};

}


/// Another solution is create factories inside the object as 'clonable' , and feed them into the clonables collenction
/// This should prevent unnesesarry construction calling and memory usage, and crashes due to the uninitialized framework

#define CLONEABLE_DECL(className)\
public: \
virtual Grafkit::Clonable* createObj() const \
	{ \
		return new className(); \
	}

#define CLONEABLE_FACTORY_DECL(className)\
	CLONEABLE_DECL(className) \
public: \
class Factory : public Grafkit::Clonable { \
public: \
	virtual Grafkit::Clonable* createObj() const \
	{ \
		return new className(); \
	} \
}; \
private: \
	static Grafkit::AddClonable _addClonableFactory;


#define CLONEABLE_FACTORY_IMPL(className) \
	Grafkit::AddClonable className##::_addClonableFactory(#className, new className##::Factory());

#define CLONEABLE_FACTORY_LOCAL_IMPL(className) \
	Grafkit::AddClonable className##_addClonableFactory(#className, new className##::Factory());

#endif //__DYNAMICS_H__
