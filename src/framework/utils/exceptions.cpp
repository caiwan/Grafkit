#include "stdafx.h"

#include <cstring>

#include <comdef.h>

#include "exceptions.h"


using namespace std;
using namespace FWdebug;

///@todo use std::string !!!

FWdebug::Exception::Exception(int errorCode, const char * message, const char * file, const char * function, int line, const char * details)
	: 
	m_formattedMessage(NULL),
	m_line(line),
	m_function(function),
	m_file(file),
	m_message(message),
	m_code(errorCode),
	m_details(NULL),
	std::exception()
{
	// copy details message due to constant fuckup
	if (details) {
		size_t slen = strlen(details);
		char* data = new char[slen + 1];
		strcpy_s(data, slen + 1, details);
		*(const_cast<char**>(&m_details)) = data;
	}
}

FWdebug::Exception::Exception(int errorCode, const char * message, const char * file, const char * function, int line, const HRESULT hresult)
	:
	m_formattedMessage(NULL),
	m_line(line),
	m_function(function),
	m_file(file),
	m_message(message),
	m_code(errorCode),
	m_details(NULL),
	std::exception()
{
	_com_error err(hresult);
	LPCTSTR details = err.ErrorMessage();

	size_t slen = strlen(details);
	char* data = new char[slen + 1];
	strcpy_s(data, slen + 1, details);
	*(const_cast<char**>(&m_details)) = data;
}

FWdebug::Exception::Exception(int errorCode, const char * message, const char * file, const char * function, int line, const DWORD dword)
	:
	m_formattedMessage(NULL),
	m_line(line),
	m_function(function),
	m_file(file),
	m_message(message),
	m_code(errorCode),
	m_details(NULL),
	std::exception()
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	size_t slen = strlen((const char*)lpMsgBuf);
	char* data = new char[slen + 1];
	strcpy_s(data, slen + 1, (const char*)lpMsgBuf);
	*(const_cast<char**>(&m_details)) = data;
}


Exception::~Exception()
{
	delete[] this->m_formattedMessage;
	delete[] * (const_cast<char**>(&m_details)); ///@todo ez valami kulonos formedveny
}

char const * FWdebug::Exception::what() const
{
	if (m_formattedMessage)
		return m_formattedMessage;

	size_t bsize = strlen(this->m_message) + 512;
	m_formattedMessage = new char[bsize];
	sprintf_s(
		this->m_formattedMessage, bsize,
		"%#x: %s in function %s in file %s, at line %d. %s", 
		this->m_code, this->m_message, this->m_function, this->m_file, 
		this->m_line, m_details ? m_details : ""
	);

	return m_formattedMessage;
}

