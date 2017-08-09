#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

#include "stdafx.h"
//#include "logger.h"	// already defied in stdafx
//#include "exceptions.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

#define BUFFER_LEGTH 64*1024

Grafkit::Logger::Logger()
{
	m_mutex = new Grafkit::Mutex;
	m_buffer = new char[BUFFER_LEGTH];

	// todo: csak az msvc outra irjon, a tobbi nem kell
	//this->AddHandler(new LoggerHandler::FileLoggerHandler("log.log", "error.log"));
	//this->AddHandler(new LoggerHandler::ConsoleLogger());
	this->AddHandler(new LoggerHandler::MsvcOutLogger());
}

Grafkit::Logger::~Logger()
{
	for (auto it = this->m_loggers.begin(); it != m_loggers.end(); it++) {
		delete *it;
	}

	delete[] m_buffer;
}

// ==================================================================================================

void Grafkit::Logger::Write(logger_msg_type_e type, const char * const message)
{
	Grafkit::MutexLocker locker(m_mutex);
	strcpy_s(m_buffer, BUFFER_LEGTH, message);
	Write(type);
}

void Grafkit::Logger::Write(logger_msg_type_e type)
{
	 size_t len = strlen(m_buffer);

	// qnd trim cr/lf at the end
	size_t k = len;
	if (k)
		while (k--)
			if (m_buffer[k] && (m_buffer[k] != '\r' || m_buffer[k] != '\n'))
				break;

	m_buffer[k+1] = 0;

	Logger::message_t message_pkg;
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

 void Grafkit::Logger::Trace(const char * const message, ...)
{
	Grafkit::MutexLocker locker(m_mutex);
	EXTRACT_VA(message, m_buffer);
	Write(LOG_TRACE);
}

 void Grafkit::Logger::Debug(const char * const message, ...)
{
	Grafkit::MutexLocker locker(m_mutex);
	EXTRACT_VA(message, m_buffer);
	Write(LOG_DEBUG);
}

 void Grafkit::Logger::Info(const char * const message, ...)
{
	Grafkit::MutexLocker locker(m_mutex);
	EXTRACT_VA(message, m_buffer);
	Write(LOG_INFO);
}

 void Grafkit::Logger::Warn(const char * const message, ...)
{
	Grafkit::MutexLocker locker(m_mutex);
	EXTRACT_VA(message, m_buffer);
	Write(LOG_WARN);
}

 void Grafkit::Logger::Error(const char * const message, ...)
{
	Grafkit::MutexLocker locker(m_mutex); 
	EXTRACT_VA(message, m_buffer);
	Write(LOG_ERROR);
}

#undef EXTRACT_VA

// ==================================================================================================
// File logger handler
// ==================================================================================================

Grafkit::LoggerHandler::FileLoggerHandler::FileLoggerHandler(const char * filename, const char * errfile) : m_stderr(nullptr), m_stdout(nullptr)
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

Grafkit::LoggerHandler::FileLoggerHandler::~FileLoggerHandler()
{
	if(this->m_stderr) fflush(this->m_stderr), fclose(this->m_stderr);
	if(this->m_stdout) fflush(this->m_stdout), fclose(this->m_stdout);
}

void Grafkit::LoggerHandler::FileLoggerHandler::Write(Grafkit::Logger::message_t * const & message)
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


Grafkit::LoggerHandler::ConsoleLogger::ConsoleLogger() : m_haveConsole(0), m_stdout(stdout), m_stderr(stderr)
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

Grafkit::LoggerHandler::ConsoleLogger::~ConsoleLogger()
{
	if (m_haveConsole)
		fclose(m_stdout);
}

void Grafkit::LoggerHandler::ConsoleLogger::Write(Grafkit::Logger::message_t * const & message)
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

Grafkit::LoggerHandler::MsvcOutLogger::MsvcOutLogger()
{
}

Grafkit::LoggerHandler::MsvcOutLogger::~MsvcOutLogger()
{
}

void Grafkit::LoggerHandler::MsvcOutLogger::Write(Grafkit::Logger::message_t * const & message)
{
	OutputDebugStringA(message->message);
	OutputDebugStringA("\n");
}
