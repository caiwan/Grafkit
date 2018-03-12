#include "LoggerProxy.h"

#include <QByteArray>
#include <QPlainTextEdit>

void Idogep::myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type) {
	case QtDebugMsg:
		Grafkit::Log::Logger().Debug("[QT] %s (%s:%u, %s)", localMsg.constData(), context.file, context.line, context.function);
		break;

	case QtInfoMsg:
		Grafkit::Log::Logger().Info("[QT] %s (%s:%u, %s)", localMsg.constData(), context.file, context.line, context.function);
		break;

	case QtWarningMsg:
		Grafkit::Log::Logger().Warn("[QT] %s (%s:%u, %s)", localMsg.constData(), context.file, context.line, context.function);
		break;

	case QtCriticalMsg:
		Grafkit::Log::Logger().Error("[QT] %s (%s:%u, %s)", localMsg.constData(), context.file, context.line, context.function);
		break;

	case QtFatalMsg:
		Grafkit::Log::Logger().Error("[QT] %s (%s:%u, %s)", localMsg.constData(), context.file, context.line, context.function);
		abort();
		break;
	}
}

namespace {
	static const char *logTexts[] = {
		"", "", "", "Warn: ", "Error: "
	};
}

void Idogep::LoggerQTAdapter::Write(Grafkit::Logger::message_t * const & message)
{
	if (!message)
		return;

	QString line = QString::fromUtf8(logTexts[message->type]) + QString::fromUtf8(message->message);
	m_list.push_back(line);

	while (m_list.size() > LOG_ROLL_SIZE)
		m_list.pop_front();

	QString logText;
	for (size_t i = 0; i < m_list.size(); i++) {
		logText += m_list.at(i) + "\r\n";
	}

	onUpdateLog(logText);

}
