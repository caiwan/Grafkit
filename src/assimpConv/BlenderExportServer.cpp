#include <cmath>
#include <time.h>

#define WIN32_LEAN_AND_MEAN

#include <fstream>

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

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
							int layerid = (int)keys["layer"];
							mat->SetLayer(layerid);
						};
					}
					catch (nlohmann::detail::exception &e) {
						// ... 
					}

					// TODO: rest of material properties

				}
			}
		}

		// ---
		json maincam = j["data"]["MainCameraMovement"];
		if (!maincam.empty()) {
			ActorAnimation* animation = new ActorAnimation();
			CameraAnimation* camAnimation = new CameraAnimation();
			CameraRef camera = new Camera();
			camera->SetName("MainCamera_1");
			ActorRef cameraActor = new Actor(camera);
			cameraActor->SetName("MainCamera");
			animation->SetActor(cameraActor);
			for (auto camit = maincam.begin(); camit != maincam.end(); camit++) {
				double t = (double)camit->at("t");
				json wm = camit->at("v")["worldmatrix"];
				// see if does not sucks
				if (m_is_lh && false) 
				{
					animation->AddPositionKey(t, float3(wm["loc"][0], wm["loc"][1], -(float)wm["loc"][2]));
					animation->AddRotationKey(t, float4(-(float)wm["rot"][3], wm["rot"][0], wm["rot"][1], wm["rot"][2]));
					animation->AddScalingKey(t, float3(wm["scale"][0], wm["scale"][1], wm["scale"][2]));
				}
				else 
				{
					animation->AddPositionKey(t, float3(wm["loc"][0], wm["loc"][1], wm["loc"][2]));
					animation->AddRotationKey(t, float4(wm["rot"][3], wm["rot"][0], wm["rot"][1], wm["rot"][2]));
					animation->AddScalingKey(t, float3(wm["scale"][0], wm["scale"][1], wm["scale"][2]));
				}
				json ca = camit->at("v")["angle"];
				camAnimation->AddFovKey(t, ca);
			}
			m_resources.cameras.push_back(camera);
			m_resources.actors["MainCamera"] = cameraActor;
			(*m_scene)->GetRootNode()->AddChild(cameraActor);
			(*m_scene)->AddAnimation(animation);
			(*m_scene)->AddAnimation(camAnimation);
		}

		// ---
		json objectanim = j["data"]["ObjectAnimations"];
		if (!objectanim.empty()) {
			for (auto objit = objectanim.begin(); objit != objectanim.end(); objit++) {
				json frames = (*objit)["frames"];
				if (!frames.empty()) {
					std::string objanme = objit->at("name");

					auto actorit = m_resources.actors.find(objanme);
					if (actorit == m_resources.actors.end())
						continue;

					json lm = objit->at("localmatrix");
					float3 pos = float3(lm["loc"][0], lm["loc"][1], lm["loc"][2]);
					float4 rot = float4(lm["rot"][3], lm["loc"][0], lm["loc"][1], lm["loc"][2]);
					float3 scale = float3(lm["scale"][0], lm["scale"][1], lm["scale"][2]);

					std::vector<AnimationRef> anims;
					ActorRef actor = actorit->second;
					(*m_scene)->GetAnimations(actor, anims);

					ActorAnimation* animation = dynamic_cast<ActorAnimation*>(anims[0].Get());

					if (!animation) {
						animation = new ActorAnimation();
						(*m_scene)->AddAnimation(animation);
					}
					else {
						animation->Clear();
					}

					animation->SetActor(actorit->second);

					for (auto frit = frames.begin(); frit != frames.end(); frit++) {
						double t = (double)frit->at("t");
						json val = frit->at("v");
						json loc_frame = val["location"];
						if (!loc_frame.empty())
							animation->AddPositionKey(t, float3(loc_frame[0], loc_frame[1], loc_frame[2]));
						else
							animation->AddPositionKey(t, pos);

						json rot_frame = val["rotation_euler"];
						if (!rot_frame.empty()) {
							Quaternion q;
							q.fromEuler(rot_frame[0], rot_frame[1], rot_frame[2]);
							animation->AddRotationKey(t, q);
						}
						else
							animation->AddRotationKey(t, rot);
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

