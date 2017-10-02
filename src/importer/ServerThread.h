#pragma once 

#include "core/thread.h"

namespace GKimporter {

	class DispatcherThread;
	class ServerThreadPrivate;

	class ServerThread {
	public:
		ServerThread(DispatcherThread* dispatcher, int port);
		~ServerThread();

		void Start() { thread->Start(); }
		void Stop() { thread->Stop(); }
		void Join() { thread->Join();  }

	private:
		Grafkit::Thread* thread; 
		ServerThreadPrivate *serverThread;
	};

}

DEFINE_EXCEPTION(ServerCreateException, 1, "Could not create server")
DEFINE_EXCEPTION(ExportMessageException, 2, "Could not parse json repsonse from server")

