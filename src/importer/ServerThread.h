#pragma once 

namespace GKimporter {

	class DispatcherThread;
	class ServerThreadPrivate;

	class ServerThread : public Grafkit::Thread {
	public:
		ServerThread(DispatcherThread* dispatcher, int port);
		~ServerThread();

	private:
		ServerThreadPrivate *serverThread;
	};

}

DEFINE_EXCEPTION(ServerCreateException, 1, "Could not create server")
DEFINE_EXCEPTION(ExportMessageException, 2, "Could not parse json repsonse from server")

