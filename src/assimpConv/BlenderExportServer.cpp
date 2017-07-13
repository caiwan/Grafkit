#include <cmath>
#include <time.h>

#define WIN32_LEAN_AND_MEAN

#include <fstream>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "json.hpp"

#include "BlenderExportServer.h"

#include "render/Scene.h"

#include "core/thread.h"
#include "utils/logger.h"

#define _cmd_initconn "hello"
#define _cmd_closeconn "goodbye"
#define _cmd_dae "collada"
#define _cmd_dump "bpydump"


using namespace FWdebugExceptions;
using namespace FWdebug;
using namespace Grafkit;
using json = nlohmann::json;

/*
https://msdn.microsoft.com/en-us/library/windows/desktop/ms737593(v=vs.85).aspx
*/

#define DEFAULT_BUFLEN 16 * 4096


/************************************************************************************************************
	Sever thread 
*************************************************************************************************************/

class ServerThread : public Grafkit::Thread {
public:

	ServerThread(BlenderExportServer* server) : terminate(false), server(server),
		ListenSocket(INVALID_SOCKET),
		ClientSocket(INVALID_SOCKET),
		recvbuflen(DEFAULT_BUFLEN)
		
	{
		recvbuf = new char[DEFAULT_BUFLEN];
	}

	~ServerThread() {
		delete[] recvbuf;
	}

private:

	void Connect() {
		int res = 0;
		char b[256];

		struct addrinfo *adinfo = NULL;
		struct addrinfo hints;

		try {
			// Initialize Winsock
			res = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (res != 0) {
				sprintf(b, "WSAStartup failed with error: %d\n", res);
				throw new EX_DETAILS(ServerCreateException, b);
			}

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			// Resolve the server address and port
			res = getaddrinfo(NULL, std::to_string(server->GetPort()).c_str(), &hints, &adinfo);
			if (res != 0) {
				sprintf(b, "getaddrinfo failed with error: %d\n", res);
				throw new EX_DETAILS(ServerCreateException, b);
			}

			// Create a SOCKET for connecting to server
			ListenSocket = socket(adinfo->ai_family, adinfo->ai_socktype, adinfo->ai_protocol);
			if (ListenSocket == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
			}

			// Setup the TCP listening socket
			res = bind(ListenSocket, adinfo->ai_addr, (int)adinfo->ai_addrlen);
			if (res == SOCKET_ERROR) {
				sprintf(b, "bind failed with error: %d\n", WSAGetLastError());
				throw new EX_DETAILS(ServerCreateException, b);
			}

		}
		catch (Exception *e) {
			if (adinfo) freeaddrinfo(adinfo);
			throw e;
		}

		if (adinfo) freeaddrinfo(adinfo);

	}

	void Accept() {
		int res = 0;
		char b[256];
		res = listen(ListenSocket, SOMAXCONN);
		if (res == SOCKET_ERROR) {
			sprintf(b, "listen failed with error: %d\n", WSAGetLastError());
			throw new EX_DETAILS(ServerCreateException, b);
		}

		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			sprintf(b, "accept failed with error: %d\n", WSAGetLastError());
			throw new EX_DETAILS(ServerCreateException, b);
		}
	}

	int Listen() {
		int res = 0;
		int recvRes = 0;
		unsigned int respLen = 0;
		res = recv(ClientSocket, (char*)&respLen, 4, 0);

		if (res > 0) {
			// recv packet
			std::stringstream ss;

			FILE* fp = NULL;
			// if (fopen_s(&fp, "debug.json", "ab") != 0) fp = NULL;

			unsigned int bytesLeft = respLen;

			while (bytesLeft > 0){
				int bytesRead = bytesLeft > recvbuflen ? recvbuflen : bytesLeft;
				
				res = recv(ClientSocket, recvbuf, bytesRead, 0);
				if (res > 0) {
					ss.write(recvbuf, bytesRead);
					
					if (fp) 
						fwrite(recvbuf, 1, bytesRead, fp);

					bytesLeft -= bytesRead;	
				}

				Log::Logger().Info("Read chunk %d %d", bytesRead, bytesLeft);
			}
			
			ss.put(0);
			ss.seekp(0);

			recvRes = server->PostData(ss);
			ss.clear();

			if (fp) {
				fflush(fp);
				fclose(fp);
			}

			if (recvRes)
				return 1;
		}

		return 0;
	}

	void TearDown() {
		if (ListenSocket != INVALID_SOCKET) closesocket(ListenSocket);
		if (ClientSocket != INVALID_SOCKET) closesocket(ClientSocket);
		WSACleanup();
	}

private:
	int Run() {
		int res = 0;

		try {
			Connect();
			Accept();
			int isDone = 0;
			do {
				isDone = Listen() != 0;
			} while (!isDone);
		}
		catch (Exception *e) {
			TearDown();
			Log::Logger().Error(e->what());
			delete e;
			res = 1;
		}

		TearDown();

		return res;
	}

	void Terminate() { terminate = true; }

private:
	bool terminate;
	BlenderExportServer *const server;

	WSADATA wsaData;

	SOCKET ListenSocket;
	SOCKET ClientSocket;

	char *recvbuf;
	int recvbuflen;


};

/************************************************************************************************************

*************************************************************************************************************/

BlenderExportServer::BlenderExportServer()
{
	m_serverThread = NULL;
	srand(time(NULL)); m_port = 1024 + rand() % 64512;
}

BlenderExportServer::~BlenderExportServer()
{
	if (m_serverThread)
		m_serverThread->Stop();
	delete m_serverThread;
}

void BlenderExportServer::Start()
{
	m_serverThread = new ServerThread(this);
	m_serverThread->Start();
}

void BlenderExportServer::Stop()
{
	if (!m_serverThread)
		return;
	m_serverThread->Join();
	delete m_serverThread;
	m_serverThread = NULL;
}

void BlenderExportServer::GetHost(std::string & str)
{
	char buf[256];
	sprintf_s(buf, "127.0.0.1:%d", m_port);
	str = buf;
}


/************************************************************************************************************
	Parse data that was recieved from Blender
*************************************************************************************************************/

bool BlenderExportServer::PostData(std::stringstream &ss) 
{
	if (m_scene.Invalid()) {
		m_scene = new Resource<Scene>(new Scene());
	}

	try 
	{
		json j = json::parse(ss);
		std::string cmd = j["cmd"];
		Log::Logger().Info((std::string("Json cmd = ") + cmd).c_str());
		
		// --- 
		if (cmd.compare(_cmd_initconn) == 0) {
			// do nothing
		} 
		
		// --- load standard collada and append it to the scene
		else if (cmd.compare(_cmd_dae) == 0) {
			std::string daefile = j["data"];
			
			char * data = new char[daefile.size()+1];
			daefile.copy(data, daefile.size() + 1);

			AppendAssimp(data, daefile.size()+1, m_scene);

		}

		// --- handle raw data daumped form the script
		else if (cmd.compare(_cmd_dump) == 0) {
			/// ... 
			std::ofstream fs("hello.json", std::ofstream::out);
			fs << j["data"];
		}
		
		// --- 
		else if (cmd.compare(_cmd_closeconn) == 0) {
			return true;
		}

	}
	catch (std::exception &e) {
		throw new EX_DETAILS(ServerCreateException, e.what());
		// Log::Logger().Error(e.what());
	}

	return false;
}

