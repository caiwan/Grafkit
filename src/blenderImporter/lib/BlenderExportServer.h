#pragma once 

#include <list>
#include <queue>

#include "core/thread.h"
#include "utils/exceptions.h"

#include "common.h"

#include "json.hpp"

#include "assimploader.h"

class ServerThread;

class BlenderExportServer : public Grafkit::Runnable, public Grafkit::AssimpLoader
{
	friend class ServerThread;
public:
	BlenderExportServer();
	~BlenderExportServer();

	void GetHost(std::string& str);

	int GetPort() { return m_port; }

	void Start();
	void Stop();	

	Grafkit::SceneResRef GetScene() {return m_scene; }

private:

	bool PostData(std::stringstream &ss);
	int Run();
	bool Parse(nlohmann::json &j);

	ServerThread *m_serverThread;
	Grafkit::Thread *m_myWorkerThread;

	int m_port;

	std::queue<nlohmann::json> m_inputDataQueue;
	Grafkit::Mutex m_inputDataQueueMutex;
	bool m_isTerminate;

	Grafkit::SceneResRef m_scene;

};

DEFINE_EXCEPTION(ServerCreateException, 1, "Could not create server")
DEFINE_EXCEPTION(ExportMessageException, 2, "Could not parse json repsonse from server")
