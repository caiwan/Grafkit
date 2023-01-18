#include "stdafx.h"

#include "exceptions.h"

#include <sstream>
#include <comdef.h>

using namespace std;
using namespace FWdebug;

Exception::Exception(int errorCode, const char * message, const char * file, const char * function, int line, const char * details)
    :
    exception(),
    m_code(errorCode),
    m_line(line),
    m_message(message),
    m_file(file),
    m_function(function),
    m_details(),
    m_formattedMessage()
{
    if (details) {
        m_details.assign(details);
    }
}

Exception::Exception(int errorCode, const char * message, const char * file, const char * function, int line, const HRESULT hresult)
    :
    exception(),
    m_code(errorCode),
    m_line(line),
    m_message(message),
    m_file(file),
    m_function(function),
    m_details(),
    m_formattedMessage()
{
    _com_error err(hresult);
    LPCTSTR details = err.ErrorMessage();

    m_details.assign(details);
}

Exception::Exception(int errorCode, const char * message, const char * file, const char * function, int line, const DWORD dword)
    :
    exception(),
    m_code(errorCode),
    m_line(line),
    m_message(message),
    m_file(file),
    m_function(function),
    m_details(),
    m_formattedMessage()
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&lpMsgBuf),
        0, nullptr);

    m_details.assign(static_cast<const char*>(lpMsgBuf));
}


Exception::~Exception()
{
}

char const * Exception::what() const
{
    if (!m_formattedMessage.empty())
        return m_formattedMessage.c_str();

    ostringstream stringStream;
    stringStream << this->m_code << ": " << this->m_message;
    stringStream << " in function " << this->m_function;
    stringStream << " in file " << this->m_file;
    stringStream << ", at line " << this->m_line;

    if (!m_details.empty())
        stringStream << " " << m_details;

    m_formattedMessage = stringStream.str();

    return m_formattedMessage.c_str();
}

