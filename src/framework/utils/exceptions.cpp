#include "stdafx.h"

#include <cstring>
#include <sstream>
#include <comdef.h>

#include "exceptions.h"


using namespace std;
using namespace FWdebug;

///@todo use std::string !!!

Exception::Exception(int errorCode, const char * message, const char * file, const char * function, int line, const char * details)
    :
    m_formattedMessage(),
    m_line(line),
    m_function(function),
    m_file(file),
    m_message(message),
    m_code(errorCode),
    m_details(),
    exception()
{
    if (details) {
        m_details.assign(details);
    }
}

Exception::Exception(int errorCode, const char * message, const char * file, const char * function, int line, const HRESULT hresult)
    :
    m_formattedMessage(),
    m_line(line),
    m_function(function),
    m_file(file),
    m_message(message),
    m_code(errorCode),
    m_details(),
    exception()
{
    _com_error err(hresult);
    LPCTSTR details = err.ErrorMessage();

    m_details.assign(details);
}

Exception::Exception(int errorCode, const char * message, const char * file, const char * function, int line, const DWORD dword)
    :
    m_formattedMessage(),
    m_line(line),
    m_function(function),
    m_file(file),
    m_message(message),
    m_code(errorCode),
    m_details(),
    exception()
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
        (LPTSTR)&lpMsgBuf,
        0, nullptr);

    m_details.assign((const char*)lpMsgBuf);
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

