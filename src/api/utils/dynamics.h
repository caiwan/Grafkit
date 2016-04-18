#ifndef __DYNAMICS_H__
#define __DYNAMICS_H__

#include <map>
#include <string>

/// @todo docstring 
/**
*/

//
/// http://www.codeproject.com/Tips/495191/Serialization-implementation-in-Cplusplus

namespace Grafkit{
	class Clonables;

	class Clonable
	{
		friend class Clonables;
		public:
			virtual Clonable* createObj() const = 0;
	};

	class Clonables {
		private:        
			typedef std::map<std::string, const Clonable*> NameToClonable;
			NameToClonable __clonables;

		private:
			Clonables(){}

			Clonables(const Clonables&);                 // Prevent copy-construction
			Clonables& operator=(const Clonables&);      // Prevent assignment
			~Clonables()
			{
				for(NameToClonable::const_iterator it = __clonables.begin(); it != __clonables.end(); ++it){
					const Clonable* clone = it->second;
					delete clone;
				}
				__clonables.clear();
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
				
				NameToClonable::const_iterator it = __clonables.find(name);
				if(it == __clonables.end()) {
					__clonables[name] = clone;
				}
			}

			const Clonable* find(const char *className)
			{
				std::string name = className;
				NameToClonable::const_iterator it = __clonables.find(name);
				if (it == __clonables.end()) 
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
	};

	class AddClonable {
		public:
			AddClonable(const char* className, Clonable* clone){
				Clonables::Instance().addClonable(className, clone);
			}
	};

}

#define CLONEABLE_DECL(className)\
virtual Grafkit::Clonable* createObj() const \
	{ \
		return new className(); \
	}

	/// Another solution is create factories inside the object as 'clonable' , and feed them into the clonables collenction
	/// This should prevent unnesesarry construction calling and memory usage, and crashes due to the uninitialized framework
#define CLONEABLE_FACTORY_DECL(className)\
class Factory : public Grafkit::Clonable { \
public: \
	virtual Grafkit::Clonable* createObj() const \
	{ \
		return new className(); \
	} \
};

#endif //__DYNAMICS_H__
