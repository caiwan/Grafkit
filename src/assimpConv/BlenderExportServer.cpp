
#define _USE_MATH_DEFINES
#include <cmath>
#include <time.h>

#define WIN32_LEAN_AND_MEAN

#include <fstream>

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include <map>

#include "BlenderExportServer.h"

#include "render/Scene.h"

#include "render/animation.h"
#include "render/camera.h"

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

BlenderExportServer::BlenderExportServer() : m_serverThread(NULL), m_myWorkerThread(NULL), m_isTerminate(false)
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
		if (fp) {
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
	try {
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
	}
	catch (Exception *& ex) {
		Log::Logger().Error(ex->what());
		delete ex;
		return 1;
	}
	return 0;
}

namespace {
	// TODO: refactor later on
	void QND_AddTextureToMaterialSafe(MaterialRef& resMat, json &j, std::string key, Material::texture_type_e tslot)
	{
		if (j[key].is_string()) {
			std::string name = j[key];
			TextureResRef t = new TextureRes();
			t->SetName(name);
			resMat->SetTexture(t, tslot);
		}
	}
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

		if (m_scene.Invalid() && m_scene->Invalid())
			throw new EX_DETAILS(AssertFailException, "Scene was not set yet");

		std::string str = j.dump();

		// --- 
		json scene = j["data"]["Scene"];

		if (!scene.empty()) {
			double frametime = (double)scene["fps_base"] / (double)scene["fps"];
			(*m_scene)->SetStartTime(frametime * (double)scene["frame_start"]);
			(*m_scene)->SetEndTime(frametime * (double)scene["frame_end"]);
		}

		//--
		json camkeys = j["data"]["CameraKeys"];
		if (!camkeys.empty()) {

			// Compromises.
			// Compromises everywhere

			std::map<std::string, int> cameraIdMap;
		
			for (int i = 0; i < (*m_scene)->GetCameraCount(); i++) {
				ActorRef a = (*m_scene)->GetCamera(i);
				std::string n = a->GetName();
				cameraIdMap[n] = i;
			}
			
			for (auto camit = camkeys.begin(); camit != camkeys.end(); camit++) {
				double t = (double)camit->at("t");
				json v = camit->at("v");
				if (!v["key"].empty()) {
					std::string key = v["key"];
					auto cidit = cameraIdMap.find(key);
					if (cidit != cameraIdMap.end()) {
						(*m_scene)->AddCurrentCameraFrame(t, cidit->second);
					}
				}
			}

		}

		// --
		json materials = j["data"]["Materials"];
		if (!materials.empty()) {
			for (auto mat_it = materials.begin(); mat_it != materials.end(); mat_it++) {
				std::string matname = mat_it->at("name");
				// TDOD : refactor this asap. see Scene.h and assimploader.h
				MaterialRef mat;
				for (auto it = m_resources.materials.begin(); it != m_resources.materials.end(); it++) {
					if (matname.compare(it->Get()->GetName()) == 0) {
						mat = *it;
						break;
					}
				}
				if (mat.Valid()) {
					try {
						std::string ds = mat_it->dump();
						json keys = mat_it->at("keys");
						if (!keys.empty()) {
							/*int layerid = (int)keys["layer"];
							mat->SetLayer(layerid);*/

							QND_AddTextureToMaterialSafe(mat, keys, "mat_normal", Material::TT_normal);
							QND_AddTextureToMaterialSafe(mat, keys, "mat_metallic", Material::TT_metallic);
							QND_AddTextureToMaterialSafe(mat, keys, "mat_roughness", Material::TT_roughness);
							QND_AddTextureToMaterialSafe(mat, keys, "mat_base_color", Material::TT_diffuse);
							QND_AddTextureToMaterialSafe(mat, keys, "mat_emission", Material::TT_emission);
						};
					}
					catch (nlohmann::detail::exception &e) {
						DebugBreak();
					}

				}
			}
		}



	}

	// --- 
	else if (cmd.compare(_cmd_closeconn) == 0) {
		// TODO: get if any errors have happened
		return false;
	}

	return true;
}

