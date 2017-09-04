#define _USE_MATH_DEFINES
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

#if 0 //MainCamera

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
				if (m_is_lh)
				{
					float3 p = float3(wm["loc"][0], wm["loc"][1], -(float)wm["loc"][2]);
					animation->AddPositionKey(t, p);
					Quaternion q = Quaternion(float4(-(float)wm["rot"][1], -(float)wm["rot"][2], wm["rot"][3], (float)wm["rot"][0]));
					animation->AddRotationKey(t, q);
					float4 lolmi = q.toAxisAngle(); //dbg
					animation->AddScalingKey(t, float3(wm["scale"][0], wm["scale"][1], wm["scale"][2]));
				}
				else
				{
					animation->AddPositionKey(t, float3(wm["loc"][0], wm["loc"][1], wm["loc"][2]));
					Quaternion q = Quaternion(float4(wm["rot"][1], wm["rot"][2], wm["rot"][3], wm["rot"][0]));
					animation->AddRotationKey(t, q);
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

#endif // Main Camnera

#if 0 // Object Baking 

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
					if (m_is_lh) pos = float3(lm["loc"][0], lm["loc"][1], -(float)lm["loc"][2]);
					Quaternion rot = Quaternion(float4(lm["rot"][1], lm["rot"][2], lm["rot"][3], lm["rot"][0]));
					if (m_is_lh) rot = Quaternion(float4(-(float)lm["rot"][1], -(float)lm["rot"][2], lm["rot"][3], lm["rot"][0]));
					float3 scale = float3(lm["scale"][0], lm["scale"][1], lm["scale"][2]);

					float4 lolmi = rot.toAxisAngle(); //dbg

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

						// This is the overall calculated local matrix relative to the actors parent
						json localmatrix = val["localmatrix"];
						if (!localmatrix.empty()) {
							json j_loc = localmatrix["loc"];
							json j_rot = localmatrix["rot"];
							json j_scale = localmatrix["scale"];

							float3 p, s;
							Quaternion q;

							if (m_is_lh) {
								p = float3(j_loc[0], j_loc[1], -(float)j_loc[2]);
								q = Quaternion(float4(-(float)j_rot[1], -(float)j_rot[2], j_rot[3], j_rot[0]));
							}
							else {
								p = float3(j_loc[0], j_loc[1], j_loc[2]);
								q = Quaternion(float4(j_rot[1], j_rot[2], j_rot[3], j_rot[0]));
							}
							s = float3(j_scale[0], j_scale[1], j_scale[2]);

							animation->AddPositionKey(t, p);
							animation->AddRotationKey(t, q);
							animation->AddScalingKey(t, s);
						}
#if 0
						// These comes right from the animation curves
						json actions = val["actions"];
						if (!actions.empty()) {
							json loc_frame = val["location"];
							if (!loc_frame.empty())
								if (m_is_lh)
									animation->AddPositionKey(t, float3(loc_frame[0], loc_frame[1], -(float)loc_frame[2]));
								else
									animation->AddPositionKey(t, float3(loc_frame[0], loc_frame[1], loc_frame[2]));

							else
								animation->AddPositionKey(t, pos);

							json rot_frame = val["rotation_euler"];
							if (!rot_frame.empty()) {
								Quaternion q = Quaternion::fromEuler(rot_frame[0], rot_frame[1], rot_frame[2]);
								if (m_is_lh) {
									float4 p = q;
									p.x *= -1.;
									p.y *= -1.;
									p = Quaternion(p);
								}
								float4 lolmi = q.toAxisAngle();
								animation->AddRotationKey(t, q);
							}
							else
								animation->AddRotationKey(t, rot);

							animation->AddScalingKey(t, scale);
						}

#endif // 0
					}

				}
			}
		}

#endif // Object baking
	}

	// --- 
	else if (cmd.compare(_cmd_closeconn) == 0) {
		// TODO: get if any errors have happened
		return false;
	}

	return true;
}

