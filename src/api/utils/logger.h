/**
	Alap, nem-threadsafe logger; csak a hibauzenetek kiirasara, legfeljeb debuggolasra

	Ezek lehet, hogy a jovoben szep es jo dolgok lesznek;
	http://stackoverflow.com/questions/14086417/how-to-write-custom-input-stream-in-c
	http://www.mr-edd.co.uk/blog/beginners_guide_streambuf
*/

#pragma once

//#define USE_LOGGER
//
//#ifndef __LOGGER_H__
//
//#ifndef USE_LOGGER
//
//#define LOGGER(x) 
//
//#else // USE_LOGGER

#define LOGGER(x) x

#include <cstdio>
#include <set>
#include <cstdlib>
#include <varargs.h>

namespace Grafkit {

	class Logger;
	class Log;

	class Logger
	{
		friend class Grafkit::Log;

	public:

		// --- 

		enum logger_msg_type_e {
			LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR
		};

		struct message_t {
			logger_msg_type_e type;
			char const * message;
			size_t length;
		};

		// --- 
		// Logger handler  class 
		class ILoggerHandler
		{
			friend class Logger;
		public:
			ILoggerHandler() {}
			~ILoggerHandler() {}
		protected:
			// pure virtual 
			virtual void Write(message_t * const & message) = 0;
		};

		// Methods 

		void AddHandler(ILoggerHandler* hdl) { this->m_loggers.insert(hdl);}
		void RemoveHandler(ILoggerHandler* hdl) { this->m_loggers.erase(this->m_loggers.find(hdl));}

		 void Write(logger_msg_type_e type, const char* const message);
		
		 void Trace(const char* const message, ... );
		 void Debug(const char* const message, ...);
		 void Info(const char* const message, ...);
		 void Warn(const char* const message, ...);
		 void Error(const char* const message, ...);

	protected:
		Logger();
		~Logger();


	private:
		typedef std::set<ILoggerHandler*> loggers_t;

		loggers_t m_loggers;

	};

	class Log {
		/// get logger; lazy singleton 
	public:
		static Logger & Logger()
		{
			static Grafkit::Logger logger;
			return logger;
		}
	};

	class LoggerHandler {
		// =======================================================================================================================================
	public:
		class FileLoggerHandler : public Logger::ILoggerHandler {
		public:
			FileLoggerHandler(const char* filename = nullptr, const char* errfile = nullptr);
			~FileLoggerHandler();
		protected:
			virtual void Write(Grafkit::Logger::message_t * const & message);
		private:
			FILE* m_stdout;
			FILE* m_stderr;
		};

		// ------------------------------------------------------------------

		class ConsoleLogger : public FileLoggerHandler {
		public:
			ConsoleLogger();
			~ConsoleLogger();
		protected:
			virtual void Write(Grafkit::Logger::message_t * const & message);

		private:
			FILE* m_stdout;
			FILE* m_stderr;
		};
	};
}

//#endif //USE_LOGGER
//#endif //__LOGGER_H__
//
