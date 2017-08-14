#include <cmath>
#include <time.h>

#define WIN32_LEAN_AND_MEAN

#include <fstream>

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

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

#include "ServerThread.inc"

/************************************************************************************************************

*************************************************************************************************************/

BlenderExportServer::BlenderExportServer() : m_serverThread (NULL) ,m_myWorkerThread (NULL), m_isTerminate(false)
{	
	srand(time(NULL)); 
	m_port = 1024 + rand() % 64512;

	m_scene = new SceneRes(new Scene());
}

BlenderExportServer::~BlenderExportServer()
{
	Stop();
}

void BlenderExportServer::Start()
{
	m_serverThread = new ServerThread(this);
	m_serverThread->Start();

	m_myWorkerThread = new Grafkit::Thread(this);
	m_myWorkerThread->Start();
}

//

void BlenderExportServer::Stop()
{
	m_isTerminate = true;

	if (m_myWorkerThread) {
		m_myWorkerThread->Join();
		delete m_myWorkerThread;
		m_myWorkerThread = NULL;
	}

	if (m_serverThread) {
		m_serverThread->Terminate();
		m_serverThread->Stop();
		delete m_serverThread;
		m_serverThread = NULL;
	}

}

void BlenderExportServer::GetHost(std::string & str)
{
	char buf[256];
	sprintf_s(buf, "127.0.0.1:%d", m_port);
	str = buf;
}

bool BlenderExportServer::PostData(std::stringstream &ss) 
{
	Grafkit::MutexLocker lock(m_inputDataQueueMutex);
	try {

		static int counter = 0;
		FILE* fp = nullptr;
		char fn[256];
		sprintf_s(fn, "dump%d", counter++);

		//fopen_s(&fp, fn, "wb");
		if (fp){
			std::string s = ss.str();
			fwrite(s.c_str(), s.length(), 1, fp);
			fflush(fp);
			fclose(fp);
		}

		json j = json::parse(ss);
		m_inputDataQueue.push(j);
	}
	catch (std::exception &e) {
		throw new EX_DETAILS(ServerCreateException, e.what());
		//Log::Logger().Error(e.what());
	}

	return !m_isTerminate;
}

int BlenderExportServer::Run()
{
	do {
		bool is_done = false;
		{
			Grafkit::MutexLocker lock(m_inputDataQueueMutex);
			is_done = m_inputDataQueue.empty();
		}
		while (!is_done)
		{
			json j;
			{
				Grafkit::MutexLocker lock(m_inputDataQueueMutex);
				j = m_inputDataQueue.front();
				m_inputDataQueue.pop();
				is_done = m_inputDataQueue.empty();
			}

			m_isTerminate = !Parse(j);
		}
	} while (!m_isTerminate);

	return 0;
}

bool BlenderExportServer::Parse(json & j)
{
	std::string cmd = j["cmd"];
	Log::Logger().Info((std::string("Json cmd = ") + cmd).c_str());

	// --- 
	if (cmd.compare(_cmd_initconn) == 0) {
		// do nothing
	}

	// --- load standard collada and append it to the scene
	else if (cmd.compare(_cmd_dae) == 0) {
		std::string daefile = j["data"];

		char * data = new char[daefile.size() + 1];
		daefile.copy(data, daefile.size() + 1);

		AppendAssimp(data, daefile.size() + 1, m_scene);

	}

	// --- handle raw data dumped right form the script
	else if (cmd.compare(_cmd_dump) == 0) {
		
		// --- 
		json scene = j["data"]["Scene"];

		if (!scene.empty()) {
			double frametime = (double)scene["fps_base"] / (double)scene["fps"];
			(*m_scene)->SetStartTime(frametime * (double)scene["frame_start"]);
			(*m_scene)->SetEndTime(frametime * (double)scene["frame_end"]);
		}

		// --
		json materials = j["data"]["Materials"];
		if (!materials.empty()) {
			for (auto mat_it = materials.begin(); mat_it != materials.end; mat_it++) {
				std::string matname = mat_it->at("name");
				// findmat

				int layerid = 0;
			}
		}
		
		// ---
		json maincam = j["data"]["MainCameraMovement"];
		if (!maincam.empty()) {

		}
	}

	// --- 
	else if (cmd.compare(_cmd_closeconn) == 0) {
		// TODO: get if any errors have happened
		return false;
	}

	return true;
}

