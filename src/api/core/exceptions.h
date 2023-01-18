#pragma once


#include <Windows.h>

namespace FWdebug {

	/// http://en.cppreference.com/w/cpp/error/exception

	/// @todo use std::String
	class Exception 
	{
	private:
		int m_code, m_line;
		LPCWCHAR m_message, m_file, m_function;
		WCHAR *m_formattedMessage;
		
	public:
		Exception(int errorCode, LPCWCHAR message, LPCWCHAR file, LPCWCHAR function, int line);
		virtual ~Exception();

		inline int getCode() { return this->m_code; }

		WCHAR* getError();

		///@todo copy constructor + assign operator letilt
	};
}


// macros that helps concatenate occurence place, and upconvert them to wchar
#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)
#define __WFILE__ WIDE1(__FILE__)
#define __WFUNCTION__ WIDE1(__FUNCTION__)

// list of parameters for occurence place 
#define EX_WHERE __WFILE__ , __WFUNCTION__, __LINE__

// wrapper for exception
#define EX(x) x(EX_WHERE)

// --------------------------------------------------------------------------------
/**
Generated exception types are listed in a separate namespace
*/
namespace FWdebugExceptions {
}

/// Macro that declares a new exceptoon type to a new namespace
#define DEFINE_EXCEPTION(_class_, _id_, _message_)\
namespace FWdebugExceptions {\
	class _class_ : public FWdebug::Exception{\
		public:\
			_class_ (const LPCWCHAR file, const LPCWCHAR function, int line) : FWdebug::Exception(_id_, WIDE1(_message_), file, function, line) {} \
	};\
}

// --------------------------------------------------------------------------------
DEFINE_EXCEPTION(NullPointerException, 1, "Null pointer exception")
