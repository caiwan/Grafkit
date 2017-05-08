#pragma once 

#include <list>

#include "core/thread.h"
#include "utils/exceptions.h"

class ServerThread;

class BlenderExportServer {
	friend class ServerThread;
public:
	BlenderExportServer();
	~BlenderExportServer();

	void GetHost(std::string& str);

	int GetPort() { return m_port; }

	void Start();
	void Stop();	

private:
	ServerThread *m_serverThread;
	int m_port;

	void PostData(void* j);

	//std::list<>
};

DEFINE_EXCEPTION(ServerCreateException, 1, "Could not create server")
