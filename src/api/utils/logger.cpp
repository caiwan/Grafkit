#include "stdafx.h"

#include "logger.h"

#include "exceptions.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

// #ifdef USE_LOGGER

using namespace Grafkit;
using namespace FWdebugExceptions;

Grafkit::Logger::Logger()
{
	this->AddHandler(new LoggerHandler::FileLoggerHandler("log.log", "error.log"));
	this->AddHandler(new LoggerHandler::ConsoleLogger());
}

Grafkit::Logger::~Logger()
{
	// clenaup
	
	for (loggers_t::iterator it = this->m_loggers.begin(); it != m_loggers.end(); it++) {
		delete *it;
	}
}

// ==================================================================================================

 void Grafkit::Logger::Write(logger_msg_type_e type, const char * const message)
{
	Logger::message_t message_pkg;
	message_pkg.length = 0;
	message_pkg.message = message;
	message_pkg.type = type;

	for (loggers_t::iterator it = this->m_loggers.begin(); it != m_loggers.end(); it++) 
	{
		if (*it) (*it)->Write(&message_pkg);
	}
}

#define EXTRACT_VA(fmt, buffer) { \
	va_list args; \
	\
	va_start(args, fmt); \
	vsprintf_s(buffer, 65536, message, args); \
	va_end(args); \
}

 void Grafkit::Logger::Trace(const char * const message, ...)
{
	char buffer[65536];	
	EXTRACT_VA(message, buffer);
	Write(LOG_TRACE, buffer);
}

 void Grafkit::Logger::Debug(const char * const message, ...)
{
	char buffer[65536];
	EXTRACT_VA(message, buffer);
	Write(LOG_DEBUG, buffer);
}

 void Grafkit::Logger::Info(const char * const message, ...)
{
	char buffer[65536];
             	EXTRACT_VA(message, buffer);
	Write(LOG_INFO, buffer);
}

 void Grafkit::Logger::Warn(const char * const message, ...)
{
	char buffer[65536];
	EXTRACT_VA(message, buffer);
	Write(LOG_WARN, buffer);
}

 void Grafkit::Logger::Error(const char * const message, ...)
{
	char buffer[65536];
	EXTRACT_VA(message, buffer);
	Write(LOG_ERROR, buffer);
}

#undef EXTRACT_VA

// ==================================================================================================
// File logger handler
// ==================================================================================================

Grafkit::LoggerHandler::FileLoggerHandler::FileLoggerHandler(const char * filename, const char * errfile) : m_stderr(nullptr), m_stdout(nullptr)
{
	if (filename) {
		if (!fopen_s(&this->m_stdout, filename, "wb")) {
			// ... 
		}
		else {
			// ... 
		}
	}
	
	if (errfile) {
		if (!fopen_s(&this->m_stderr, errfile, "wb")) {
			// ... 
		}
		else {
			// ... 
		}
	}
}

Grafkit::LoggerHandler::FileLoggerHandler::~FileLoggerHandler()
{
	if(this->m_stderr) fflush(this->m_stderr), fclose(this->m_stderr);
	if(this->m_stdout) fflush(this->m_stdout), fclose(this->m_stdout);
}

void Grafkit::LoggerHandler::FileLoggerHandler::Write(Grafkit::Logger::message_t * const & message)
{
	if (!message)
		return;

	if (message->type == Logger::LOG_ERROR || message->type == Logger::LOG_WARN) {
		if (this->m_stderr) 
			fprintf_s(this->m_stderr, "%s\r\n", message->message);
	}
	/*else*/ {
		if(this->m_stdout) 
			fprintf_s(this->m_stdout, "%s\r\n", message->message);
	}
}

// #endif // USE_LOGGER

// ==================================================================================================
// Console logger handler
// ==================================================================================================


Grafkit::LoggerHandler::ConsoleLogger::ConsoleLogger()
{
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	//FILE* hf_out = _fdopen(hCrt, "w");
	m_stdout = m_stderr = _fdopen(hCrt, "w");
	setvbuf(m_stdout, NULL, _IONBF, 1);
	// *stdout = *hf_out;
}

Grafkit::LoggerHandler::ConsoleLogger::~ConsoleLogger()
{
	fclose(m_stdout);
}

void Grafkit::LoggerHandler::ConsoleLogger::Write(Grafkit::Logger::message_t * const & message)
{
	if (!message)
		return;

	if (message->type == Logger::LOG_ERROR || message->type == Logger::LOG_WARN) {
		if (this->m_stderr)
			fprintf_s(this->m_stderr, "%s\r\n", message->message);
	}
	else {
		if (this->m_stdout)
			fprintf_s(this->m_stdout, "%s\r\n", message->message);
	}
}
