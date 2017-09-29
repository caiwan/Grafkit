#pragma once 

namespace GKimporter {

	class BlenderThread;
	class ServerThread;
	class DispatcherThread;

	class Environment;
	
	class ImporterTool {
	public:
		ImporterTool();
		virtual ~ImporterTool();

		Environment* Env() { return env; }

		int GetPort() { return port; }
		std::string GetHost();

		void Start();
		void Join();
		void Stop();

		virtual int Execute(int argc, char* argv[]) = 0;

	protected:
		BlenderThread *blender;
		ServerThread *server;
		DispatcherThread *dispatch;

		Environment *env;

	private:
		int port;
	
	};
}