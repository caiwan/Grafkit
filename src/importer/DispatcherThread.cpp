#include "DispatcherThread.h"

#include "Command.h"

using namespace Grafkit;
using namespace GKimporter;
using namespace FWdebug;
using namespace FWdebugExceptions;
using json = nlohmann::json;

#define _cmd_initconn "hello"
#define _cmd_closeconn "goodbye"
#define _cmd_log "log"

GKimporter::DispatcherThread::DispatcherThread(Environment * env) : Grafkit::Thread(), env(env), isTerminate(false)
{
}

void GKimporter::DispatcherThread::AddCommand(Ref<Command> command)
{
	commands[command->GetCommandString()] = command;
}

bool GKimporter::DispatcherThread::PostData(std::stringstream & ss)
{
	Grafkit::MutexLocker lock(inputDataQueueMutex);
	try {

		static int counter = 0;
		FILE* fp = nullptr;
		char fn[256];

		json j = json::parse(ss);
		m_inputDataQueue.push(j);
	}
	catch (std::exception &e) {
		throw new EX_DETAILS(DispatcherExcerption, e.what());
	}

	return !isTerminate;
}

bool GKimporter::DispatcherThread::Parse(nlohmann::json & j)
{
	std::string commandString = j["cmd"];
	Log::Logger().Info((std::string("Json cmd = ") + commandString).c_str());

	json data = j["data"];

	// init conn
	if (commandString.compare(_cmd_initconn) == 0) {
		Log::Logger().Info("Handshake ok");
		return true;
	}
	// close conn
	else if (commandString.compare(_cmd_closeconn) == 0) {
		Log::Logger().Info("Terminate connection ok");
		return false;
	}

	// messages
	else if (commandString.compare(_cmd_log) == 0) {
		std::string message = data["message"];
		Log::Logger().Info("CLIENT: %s", message.c_str());
	}

	// dispatch the rest of packets
	else {
		auto command = commands.find(commandString);
		if (command != commands.end()) {
			command->second->Evaluate(env, data);
		}
		return false;
	}


	return true;
}

int GKimporter::DispatcherThread::Run()
{
	Log::Logger().Info("Dispatcher start");

	try {
		do {
			bool is_done = false;
			{
				Grafkit::MutexLocker lock(inputDataQueueMutex);
				is_done = m_inputDataQueue.empty();
			}
			while (!is_done)
			{
				json j;
				{
					Grafkit::MutexLocker lock(inputDataQueueMutex);
					j = m_inputDataQueue.front();
					m_inputDataQueue.pop();
					is_done = m_inputDataQueue.empty();
				}

				isTerminate = !Parse(j);
			}
		} while (!isTerminate);
	}
	catch (Exception *& ex) {
		Log::Logger().Error(ex->what());
		delete ex;
		return 1;
	}
	return 0;
}
