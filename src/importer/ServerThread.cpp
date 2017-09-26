#include <winsock2.h>
#include <ws2tcpip.h>

/*
https://msdn.microsoft.com/en-us/library/windows/desktop/ms737593(v=vs.85).aspx
*/

#define DEFAULT_BUFLEN 16 * 4096

/************************************************************************************************************
	Sever thread clazz
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
		catch (Exception *&e) {
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

			static int c = 0;
			char b[2456];
			sprintf_s(b, "dump_recv%d", c++);

			FILE* fp = NULL;
			// if (fopen_s(&fp, b, "ab") != 0) fp = NULL;

			unsigned int bytesLeft = respLen;

			while (bytesLeft > 0){
				int bytesRead = bytesLeft > recvbuflen ? recvbuflen : bytesLeft;
				
				res = recv(ClientSocket, recvbuf, bytesRead, 0);
				if (res > 0) {
					ss.write(recvbuf, res);
					
					if (fp) 
						fwrite(recvbuf, 1, res, fp);

					bytesLeft -= res;	
				}

				Log::Logger().Info("Read chunk %d %d", bytesRead, bytesLeft);
			}
			
			Log::Logger().Info("Done read %d %d", respLen, ss.tellg());

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
			do {
				Listen();
			} while (!terminate);
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

public:
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
