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
	}
}

namespace {
	const char *logTexts[] = {
		"", "", "", "Warn: ", "Error: "
	};
}

void Idogep::LoggerProxy::Write(const Grafkit::Logger::message_t & message)
{
	if (message.type == Grafkit::Logger::LOG_DEBUG)
		return;

	std::string line = std::string(logTexts[message.type]) + std::string(message.message);
	onUpdateLog(line);

}
