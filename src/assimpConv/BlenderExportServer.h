#pragma once 

#include "core/thread.h"
#include "utils/exceptions.h"

class ServerThread;

class BlenderExportServer {
public:
	BlenderExportServer();
	~BlenderExportServer();

	void GetHost(std::string& str);

	void Start();
	void Stop();

private:
	ServerThread *m_serverThread;
	int m_port;

	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
};

DEFINE_EXCEPTION(ServerCreateException, 1, "Could not create server")
