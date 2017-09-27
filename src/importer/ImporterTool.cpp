#include "ImporterTool.h"

#include "BlenderThread.h"

using namespace GKimporter;


GKimporter::ImporterTool::ImporterTool()
{
	blenderThread = new BlenderThread();
}

GKimporter::ImporterTool::~ImporterTool()
{
	if (blenderThread) {
		blenderThread->Stop();
		delete blenderThread;
	}
}
