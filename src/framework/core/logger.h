/**
	Note: [hun]
	Alap, nem-threadsafe logger; csak a hibauzenetek kiirasara, legfeljeb debuggolasra

	Ezek lehet, hogy a jovoben szep es jo dolgok lesznek;
	http://stackoverflow.com/questions/14086417/how-to-write-custom-input-stream-in-c
	http://www.mr-edd.co.uk/blog/beginners_guide_streambuf
*/

#pragma once

#define HAS_LOGGER 1
#define LOGGER(x) x

constexpr size_t loggerBufferLength = 64 * 1024;


#include <array>
#include <set>

namespace Grafkit {

	class Mutex;

	class Logger;
	class Log;

	/*
	QnD logger handler
	*/

	class Logger
	{
		friend class Log;

	public:

		// --- 

		enum logger_msg_type_e {
			LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, _LOG_COUNT
		};

		struct message_t {
			logger_msg_type_e type;
			char const * message;
			size_t length;
		};

		// --- 
		/**
		Logger interface
		*/
		class ILoggerHandler
		{
			friend class Logger;
		public:
			ILoggerHandler() {}
		    virtual ~ILoggerHandler() {}
			virtual void Write(const message_t & message) = 0;
		};

		// Methods, ops
		void SetLogLevel(logger_msg_type_e type, bool isShow) { m_hideMessage[type] = !isShow; }
		void AddHandler(std::shared_ptr<ILoggerHandler> hdl) { this->m_loggers.insert(hdl);}
		//void RemoveHandler(std::unique_ptr<ILoggerHandler> hdl) { this->m_loggers.erase(this->m_loggers.find(hdl));}

		 void Write(logger_msg_type_e type, const char* message);
		
		 void Trace(const char* const message, ... );
		 void Debug(const char* const message, ...);
		 void Info(const char* const message, ...);
		 void Warn(const char* const message, ...);
		 void Error(const char* const message, ...);

	protected:
		void Write(logger_msg_type_e type);

		Logger();
		~Logger();


	private:
		std::set<std::shared_ptr<ILoggerHandler>> m_loggers;
        std::array<char, loggerBufferLength> m_buffer;
        std::unique_ptr<Mutex> m_mutex;

		bool m_hideMessage[_LOG_COUNT];
	};

	/*
		Lazy sginglewton for global logger handler
	*/
	class Log {
	public:
		static Logger & Logger()
		{
			static Grafkit::Logger logger;
			return logger;
		}
	};

	// =======================================================================================================================================
	
	/*
		Predefined logger handlers
	*/

	class LoggerHandler {
	public:

		// ------------------------------------------------------------------
		class FileLoggerHandler : public Logger::ILoggerHandler {
		public:
			explicit FileLoggerHandler(const char* filename = nullptr, const char* errfile = nullptr);
			~FileLoggerHandler();
		protected:
            void Write(const Logger::message_t & message) override;
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
		    void Write(const Logger::message_t & message) override;

		private:
			FILE* m_stdout;
			FILE* m_stderr;

			int m_haveConsole;
		};

		// ------------------------------------------------------------------

		class MsvcOutLogger : public FileLoggerHandler{
		public:
			MsvcOutLogger();
			~MsvcOutLogger();
		protected:
            void Write(const Logger::message_t & message) override;
		};

	};
}

//#endif //USE_LOGGER
//#endif //__LOGGER_H__
//
