#include "stdafx.h"

#include "exceptions.h"

#include <cstring>
#include <cstdio>

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

