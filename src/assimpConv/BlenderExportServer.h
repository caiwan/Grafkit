#pragma once 

#include <list>

#include "core/thread.h"
#include "utils/exceptions.h"

#include "common.h"

#include "assimploader.h"

class ServerThread;

class BlenderExportServer : public Grafkit::AssimpLoader
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
	ServerThread *m_serverThread;
	int m_port;

	bool PostData(std::stringstream &ss);

	Grafkit::SceneResRef m_scene;
};

DEFINE_EXCEPTION(ServerCreateException, 1, "Could not create server")
DEFINE_EXCEPTION(ExportMessageException, 2, "Could not parse json repsonse from server")
