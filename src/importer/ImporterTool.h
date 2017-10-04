#pragma once 

#include <string>

namespace GKimporter {

	class BlenderThread;
	class ServerThread;
	class DispatcherThread;

	class Environment;

	class ImporterTool {
	public:
		ImporterTool* Instance() { return self; }

	public:
		ImporterTool();
		virtual ~ImporterTool();

		Environment* Env() { return env; }

		int GetPort() { return port; }
		std::string GetHost();

		void Start();
		void Join();
		void Stop();

		void Build();
		//void Save();

		virtual int Execute(int argc, char* argv[]) = 0;

	protected:
		BlenderThread *blender;
		ServerThread *server;
		DispatcherThread *dispatch;

		Environment *env;

		static ImporterTool * self;

	private:
		int port;

	};
}