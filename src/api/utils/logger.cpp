#include "logger.h"

#include "exceptions.h"

#ifdef USE_LOGGER

using namespace Grafkit;
using namespace FWdebugExceptions;

Grafkit::Logger::Logger()
{
}

Grafkit::Logger::~Logger()
{
}

// ==================================================================================================

inline void Grafkit::Logger::Write(logger_msg_type_e type, const char * const message)
{

}

#define EXTRACT_VA(fmt, buffer) { \
	va_list args; \
	\
	va_start(args, format); \
	vsprintf_s(buffer, 65536, message, args); \
	va_end(args); \
}

inline void Grafkit::Logger::Trace(const char * const message, ...)
{
	char buffer[65536];	
	EXTRACT_VA(message, buffer);
	Write(LOG_TRACE, buffer);
}

inline void Grafkit::Logger::Debug(const char * const message, ...)
{
	char buffer[65536];
	EXTRACT_VA(message, buffer);
	Write(LOG_DEBUG, buffer);
}

inline void Grafkit::Logger::Info(const char * const message, ...)
{
	char buffer[65536];
	EXTRACT_VA(message, buffer);
	Write(LOG_INFO, buffer);
}

inline void Grafkit::Logger::Warn(const char * const message, ...)
{
	char buffer[65536];
	EXTRACT_VA(message, buffer);
	Write(LOG_WARN, buffer);
}

inline void Grafkit::Logger::Error(const char * const message, ...)
{
	char buffer[65536];
	EXTRACT_VA(message, buffer);
	Write(LOG_ERROR, buffer);
}

#undef EXTRACT_VA

// ==================================================================================================
// Logger handler
// ==================================================================================================

void Grafkit::Logger::ILoggerHandler::Update(Observable * source, void * data)
{
	message_t* message = (message_t*)(data);
	Write(message);
}

// ==================================================================================================
// File logger handler
// ==================================================================================================

Grafkit::FileLoggerHandler::FileLoggerHandler(const char * filename, const char * errfile) : m_stderr(nullptr), m_stdout(nullptr)
{
	if (filename) {
	}
	
	if (errfile) {
	}
}

Grafkit::FileLoggerHandler::FileLoggerHandler(const char * filename, const char * errfile)
{
}

Grafkit::FileLoggerHandler::~FileLoggerHandler()
{
	if(this->m_stderr) fflush(this->m_stderr), fclose(this->m_stderr);
	if(this->m_stdout) fflush(this->m_stderr), fclose(this->m_stderr);
}

void Grafkit::FileLoggerHandler::Write(Grafkit::Logger::message_t * const & message)
{
	// if (this->fp && message) fwrite(message->message, 1, message->length, this->fp), fflush(this->fp);
}

#endif // USE_LOGGER
