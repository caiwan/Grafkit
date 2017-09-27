#pragma once 

namespace GKimporter {

	class BlenderThread;
	class ServerThread;
	
	class ImporterTool {
	public:
		ImporterTool();
		virtual ~ImporterTool();

		virtual int Execute(int argc, char* argv[]) = 0;

	protected:
		BlenderThread *blenderThread;
		ServerThread *serverThread;
	};
}