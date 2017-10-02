#pragma once 

#include <list>
#include <queue>

#include "json.hpp"
#include "core/thread.h"

namespace GKimporter {

	class ServerThreadPrivate;

	class Environment;
	class Command;

	class DispatcherThread : public Grafkit::Thread{
		friend class ServerThreadPrivate;
	public:
		DispatcherThread(Environment* env);

		void AddCommand(Ref<Command> command);

		bool IsTemrinate() { return isTerminate; }

	protected:
		bool PostData(std::stringstream &ss);
		bool Parse(nlohmann::json &j);

	private:
		int Run();

	private:
		std::map<std::string, Ref<Command>> commands;
		Environment *env;

		std::queue<nlohmann::json> m_inputDataQueue;
		Grafkit::Mutex inputDataQueueMutex;

		bool isTerminate;

	};
}

DEFINE_EXCEPTION(DispatcherExcerption, 1, "Could not create server")
