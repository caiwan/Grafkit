#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <exception>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#endif // _WIN32

#define HAVE_EXCEPTIONS

namespace FWdebug {

	/// http://en.cppreference.com/w/cpp/error/exception
	/// http://www.boost.org/doc/libs/1_59_0/libs/exception/doc/boost-exception.html
	/// http://www.boost.org/community/error_handling.html

	class Exception : public std::exception
	{
	private:
		int m_code, m_line;
		std::string m_message, m_file, m_function, m_details;
		mutable std::string m_formattedMessage;
		
	public:
		Exception(int errorCode, const char* message, const char* file, const char* function, int line, const char* deatils=NULL);
		Exception(int errorCode, const char* message, const char* file, const char* function, int line, const HRESULT hresult);
		Exception(int errorCode, const char* message, const char* file, const char* function, int line, const DWORD dword);
		virtual ~Exception();

		inline int getErrorCode() { return this->m_code; }
		char const* what() const override;
	};
}


// macros that helps concatenate occurence place, and upconvert them to wchar
# if 0 // temporaly takeout 
#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)
#define __WFILE__ WIDE1(__FILE__)
#define __WFUNCTION__ WIDE1(__FUNCTION__)

// list of parameters for occurence place 
#define EX_WHERE __WFILE__ , __WFUNCTION__, __LINE__

// wrapper for exception
#define THROW_EX(x) x(EX_WHERE)
#define THROW_EX_DETAILS(x, details) x(EX_WHERE, details)

#else

#define WIDE1(x) x
// list of parameters for occurence place 
#define EX_WHERE __FILE__ , __FUNCTION__, __LINE__

// wrapper for exception
#define THROW_EX(x) throw x(EX_WHERE)
#define THROW_EX_DETAILS(x, details) throw x(EX_WHERE, details)
#define THROW_EX_HRESULT(x, hresult) throw x(EX_WHERE, hresult)
#define THROW_EX_DWORD(x, dword) throw x(EX_WHERE, dword)

# endif // temporaly takeout 

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
			_class_ (const char* file, const char* function, int line, const char* details = nullptr) : FWdebug::Exception(_id_, WIDE1(_message_), file, function, line, details) {} \
			_class_ (const char* file, const char* function, int line, const HRESULT hresult) : FWdebug::Exception(_id_, WIDE1(_message_), file, function, line, hresult) {} \
			_class_ (const char* file, const char* function, int line, const DWORD dword) : FWdebug::Exception(_id_, WIDE1(_message_), file, function, line, dword) {} \
	};\
}

// --------------------------------------------------------------------------------
DEFINE_EXCEPTION(NullPointerException, 1, "Null pointer")
DEFINE_EXCEPTION(AssertFailException, 1, "Assertion failure")
DEFINE_EXCEPTION(InvalidFormatException, 2, "Invalid format")
DEFINE_EXCEPTION(InvalidOperationException, 3, "Invalid operation committed")
DEFINE_EXCEPTION(InvalidParameterException, 4, "Invalid parameter")
DEFINE_EXCEPTION(ResourceNotFound, 5, "Resource or asset not found")
DEFINE_EXCEPTION(FileNotFoundException, 6, "File not found")
DEFINE_EXCEPTION(OutOfBoundsException, 7, "Index out of bounds");
DEFINE_EXCEPTION(NotImplementedMethodException, 8, "Method that being called has no implementation");

#ifndef LIVE_RELEASE
#define ASSERT(EXP) if ((EXP) != 0) throw new THROW_EX(FWdebugExceptions::AssertFailException)
#define ASSERT_DETAILS(EXP, REASON) if ((EXP) != 0) throw new THROW_EX_DETAILS(FWdebugExceptions::AssertFailException, REASON)
#else //LIVE_RELEASE
#define ASSERT(EXP) 
#define ASSERT_DETAILS(EXP, REASON) 
#endif //LIVE_RELEASE

#endif // __EXCEPTIONS_H__