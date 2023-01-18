#pragma once 

#include <QtGlobal>
#include <QList>
#include <QString>

#include "core/logger.h"

#include "utils/Event.h"

class QPlainTextEdit;

#define LOG_ROLL_SIZE 4096

namespace Idogep {
	extern void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

	class LoggerProxy : public Grafkit::Logger::ILoggerHandler
	{
	public:
		LoggerProxy() {}
		virtual ~LoggerProxy() {}
		virtual void Write(Grafkit::Logger::message_t * const & message);

		Event<std::string> onUpdateLog;

	private:
	};

}
