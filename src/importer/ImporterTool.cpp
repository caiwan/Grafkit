#include <random>

#include <ctime>

#include "ImporterTool.h"

#include "Command.h"
#include "Environment.h"

#include "BlenderThread.h"
#include "DispatcherThread.h"
#include "ServerThread.h"

#include "assimploader.h"

using namespace GKimporter;


GKimporter::ImporterTool::ImporterTool() : blender(nullptr), dispatch(nullptr), server(nullptr)
{
	env = new Environment();

	srand(time(NULL));
	port = 1024 + rand() % 64512;

	blender = new BlenderThread();
	dispatch = new DispatcherThread(env);
	server = new ServerThread(dispatch, port);

	dispatch->AddCommand(new AssimpLoader());
}

GKimporter::ImporterTool::~ImporterTool()
{
	Stop();
}

std::string GKimporter::ImporterTool::GetHost()
{
	char buf[256];
	sprintf_s(buf, "localhost:%d", port);
	return std::string(buf);
}

void GKimporter::ImporterTool::Start()
{
	server->Start();
	dispatch->Start();

	blender->Start();
}

void GKimporter::ImporterTool::Join()
{
	if (blender)
		blender->Join();

	if (dispatch && server)

		if (!dispatch->IsTemrinate()) {
			dispatch->Stop();
			server->Stop();
		}
		else {

			dispatch->Join();
			//server->Join();
			server->Stop(); // fuck you then
		}
}

void GKimporter::ImporterTool::Stop()
{
	if (blender) {
		blender->Stop();
		delete blender;
		blender = nullptr;
	}

	if (dispatch) {
		dispatch->Stop();
		delete dispatch;
		dispatch = nullptr;
	}

	if (server) {
		server->Stop();
		delete server;
		server = nullptr;
	}
}
