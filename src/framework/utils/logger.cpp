#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#ifndef NOMINAMX
#define NOMINMAX
#endif //NOMINMAX

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "stdafx.h"
//#include "logger.h"	// already defied in stdafx
//#include "exceptions.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

#define BUFFER_LEGTH 64*1024

Logger::Logger()
{
	m_mutex = new Mutex;
	m_buffer = new char[BUFFER_LEGTH];

	for (size_t i = 0; i < _LOG_COUNT; i++) {
		m_hideMessage[i] = false;
	}

	this->AddHandler(new LoggerHandler::MsvcOutLogger());
}

Logger::~Logger()
{
	for (auto it = this->m_loggers.begin(); it != m_loggers.end(); it++) {
		delete *it;
	}

	delete[] m_buffer;
}

// ==================================================================================================

void Logger::Write(logger_msg_type_e type, const char * const message)
{
	MutexLocker locker(m_mutex);
	strcpy_s(m_buffer, BUFFER_LEGTH, message);
	Write(type);
}

void Logger::Write(logger_msg_type_e type)
{
	// skip if log messahe type was disabled
	if (m_hideMessage[type])
		return;

	// do the rest
	size_t len = strlen(m_buffer);

	// qnd trim cr/lf at the end
	size_t k = len;
	if (k)
		while (k--)
			if (m_buffer[k] && (m_buffer[k] != '\r' || m_buffer[k] != '\n'))
				break;

	m_buffer[k + 1] = 0;

	message_t message_pkg;
	message_pkg.length = k;
	message_pkg.message = m_buffer;
	message_pkg.type = type;

	for (auto it = this->m_loggers.begin(); it != m_loggers.end(); it++)
	{
		if (*it) (*it)->Write(&message_pkg);
	}
}

#define EXTRACT_VA(fmt, buffer) { \
	va_list args; \
	\
	va_start(args, fmt); \
	vsprintf_s(buffer, BUFFER_LEGTH, message, args); \
	va_end(args); \
}

void Logger::Trace(const char * const message, ...)
{
	MutexLocker locker(m_mutex);
	EXTRACT_VA(message, m_buffer);
	Write(LOG_TRACE);
}

void Logger::Debug(const char * const message, ...)
{
	MutexLocker locker(m_mutex);
	EXTRACT_VA(message, m_buffer);
	Write(LOG_DEBUG);
}

void Logger::Info(const char * const message, ...)
{
	MutexLocker locker(m_mutex);
	EXTRACT_VA(message, m_buffer);
	Write(LOG_INFO);
}

void Logger::Warn(const char * const message, ...)
{
	MutexLocker locker(m_mutex);
	EXTRACT_VA(message, m_buffer);
	Write(LOG_WARN);
}

void Logger::Error(const char * const message, ...)
{
	MutexLocker locker(m_mutex);
	EXTRACT_VA(message, m_buffer);
	Write(LOG_ERROR);
}

#undef EXTRACT_VA

// ==================================================================================================
// File logger handler
// ==================================================================================================

LoggerHandler::FileLoggerHandler::FileLoggerHandler(const char * filename, const char * errfile) : m_stderr(nullptr), m_stdout(nullptr)
{
	if (filename) {
		if (!fopen_s(&this->m_stdout, filename, "wb")) {
			// ... throw err
		}
	}

	if (errfile) {
		if (!fopen_s(&this->m_stderr, errfile, "wb")) {
			// ... throw err
		}
	}
}

LoggerHandler::FileLoggerHandler::~FileLoggerHandler()
{
	if (this->m_stderr) fflush(this->m_stderr), fclose(this->m_stderr);
	if (this->m_stdout) fflush(this->m_stdout), fclose(this->m_stdout);
}

void LoggerHandler::FileLoggerHandler::Write(Logger::message_t * const & message)
{
	if (!message)
		return;

	if (!message->length)
		return;

	if (message->type == Logger::LOG_ERROR || message->type == Logger::LOG_WARN) {
		if (this->m_stderr) {
			fprintf_s(this->m_stderr, "%s\n", message->message);
		}
	}
	/*else*/
	{
		if (this->m_stdout) {
			fprintf_s(this->m_stdout, "%s\n", message->message);
		}
	}
}

// #endif // USE_LOGGER

// ==================================================================================================
// Console logger handler
// ==================================================================================================


LoggerHandler::ConsoleLogger::ConsoleLogger() : m_haveConsole(0), m_stdout(stdout), m_stderr(stderr)
{
#if 1
	AllocConsole(); m_haveConsole = 1;

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	//FILE* hf_out = _fdopen(hCrt, "w");
	m_stdout = m_stderr = _fdopen(hCrt, "w");
	setvbuf(m_stdout, NULL, _IONBF, 1);
	// *stdout = *hf_out;
#endif
}

LoggerHandler::ConsoleLogger::~ConsoleLogger()
{
	if (m_haveConsole)
		fclose(m_stdout);
}

void LoggerHandler::ConsoleLogger::Write(Logger::message_t * const & message)
{
	if (!message)
		return;

	if (m_haveConsole)
		if (message->type == Logger::LOG_ERROR || message->type == Logger::LOG_WARN) {
			if (this->m_stderr)
				fprintf_s(this->m_stderr, "%s\n", message->message);
		}
		else {
			if (this->m_stdout)
				fprintf_s(this->m_stdout, "%s\n", message->message);
		}
}

// ==================================================================================================
// MSVC console output logger handler
// ==================================================================================================

LoggerHandler::MsvcOutLogger::MsvcOutLogger()
{
}

LoggerHandler::MsvcOutLogger::~MsvcOutLogger()
{
}

void LoggerHandler::MsvcOutLogger::Write(Logger::message_t * const & message)
{
	OutputDebugStringA(message->message);
	OutputDebugStringA("\n");
}
