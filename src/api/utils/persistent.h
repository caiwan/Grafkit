#pragma once

#include "../../debug/exceptions.h"
#include "../../math/vectors.h"
#include "dynamics.h"

#include "../array.h"

// http://www.codeproject.com/Tips/495191/Serialization-implementation-in-Cplusplus

//#define NOT_USING_RTTI

#define PERSISTENT_SIGNATURE "CAI_OBJPK"
#define PERSISTENT_MAJOR_VER 0
#define PERSISTENT_MINOR_VER 1

namespace FWutils{
	class Archive;

	class Persistent : public Clonable
	{
		public:
			virtual ~Persistent() {}
			static Persistent* load(Archive& stream);
			void store(Archive& stream) const;

		protected:
			virtual void serialize(Archive& stream){throw new NotSupportedException(); }

			///@todo use typeid(T).name() instead, and do the lookup by that;
			virtual const char* getClassName() const = 0;
			virtual int version() const { return 0; }
	};

	class Archive
	{
		public:
			Archive(bool isStoring = false); // : _isStoring(isStoring) {}
			virtual ~Archive();// {}

		protected:
			/// minden leszarmazott konstruktoraban meg kell hivni
			/// kiirja/beolvassa a headert, es verzio ellenorzest vegez, ha kell
			void initCheck();
			
			/// I/O stuff
			virtual void write(const void* buffer, size_t length) = 0;
			virtual void read(void* buffer, size_t length) = 0;
	
		public:
			///@todo: ArrayPool<T> store/load support
			
			///@todo: BinaryTree<T> store/load  support
			///@todo: ChainTree<T> store/load support
			///@todo: ListTree<T> store/load support

			Archive& operator<< (const std::string& str);
			Archive& operator>> (std::string& str);

			Archive& operator<< (const char*& str);
			Archive& operator>> (char*& str);


			// ezzel megkonnyitem most a copypastet.

			//@todo: itt, az olvasasnal csekkolni kene a size erteket, es exceptiont dobalni igeny eseten
#define DECL_IO_ARRAY_OPERATION(type) \
	inline Archive& operator<< (const ArrayPool<type> &_array) { \
		*this<<_array.size(); \
		write(_array.get(), _array.realsize()); \
		return *this; \
	} \
	\
	inline Archive& operator>> (ArrayPool<type>& _array){ \
		int size = -1; *this>>size; \
		_array.rewind(), _array.insert(size); \
		read(_array.get(), _array.realsize()); \
		return *this;\
	}\

#define DECL_IO_OPERATION(type) \
	inline Archive& operator<< (const type &val)  {write(&val, sizeof(type)); return *this;} \
	inline Archive& operator>> (type& val) {read (&val, sizeof(type)); return *this;} \
	DECL_IO_ARRAY_OPERATION(type) 

			DECL_IO_OPERATION(char);
			DECL_IO_OPERATION(short);
			DECL_IO_OPERATION(int);
			DECL_IO_OPERATION(long);

			DECL_IO_OPERATION(unsigned char);
			DECL_IO_OPERATION(unsigned short);
			DECL_IO_OPERATION(unsigned int);
			DECL_IO_OPERATION(unsigned long);

			DECL_IO_OPERATION(double);
			DECL_IO_OPERATION(float);

			// vector support, mert miret ne
			DECL_IO_OPERATION(vector2d<char>);
			DECL_IO_OPERATION(vector3d<char>);
			DECL_IO_OPERATION(vector4d<char>);

			DECL_IO_OPERATION(vector2d<int>);
			DECL_IO_OPERATION(vector3d<int>);
			DECL_IO_OPERATION(vector4d<int>); // tobb nem kell ebbol

			DECL_IO_OPERATION(vec2float);
			DECL_IO_OPERATION(vec3float);
			DECL_IO_OPERATION(vec4float);

			DECL_IO_OPERATION(vec2double);
			DECL_IO_OPERATION(vec3double);
			DECL_IO_OPERATION(vec4double);

			bool isStoring() const { return _isStoring; }
			void setDirection(bool isStoring) { _isStoring = isStoring; }

		private: 
			int _isStoring;		// fix'd
			short _major;
			short _minor;
	};
}


// ------------------

#define PERSISTENT_DECL(className, VERSION_NO) \
	public: \
			virtual FWutils::Clonable* createObj() const \
		{ \
			return new className(); \
		} \
		CLONEABLE_FACTORY_DECL(className)\
		virtual const char* getClassName() const\
		{ \
			return #className; \
		} \
		virtual int version() const \
		{ \
			return VERSION_NO;\
		}\
	private: \
		static FWutils::AddClonable _addClonable;

#define PERSISTENT_IMPL(className) \
	FWutils::AddClonable className::_addClonable(#className, new className##::Factory());