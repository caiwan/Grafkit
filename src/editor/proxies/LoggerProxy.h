#pragma once 

#include <QtGlobal>
#include <QList>
#include <QString>

#include "utils/logger.h"

#include "Event.h"

class QPlainTextEdit;

#define LOG_ROLL_SIZE 4096

namespace Idogep {
	extern void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

	// delegate toward log messages to QT components
	class LoggerQTAdapter : public Grafkit::Logger::ILoggerHandler
	{
	public:
		LoggerQTAdapter() {}
		virtual ~LoggerQTAdapter() {}
		virtual void Write(Grafkit::Logger::message_t * const & message);

		Event<QString> onUpdateLog;

	private:
		QList<QString> m_list;

	};

}
