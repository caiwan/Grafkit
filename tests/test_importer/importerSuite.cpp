#include "importerSuite.h"

#include "ImporterTool.h"
#include "BlenderThread.h"
#include "ServerThread.h"
#include "DispatcherThread.h"

ImporterSuite::ImporterSuite() : ImporterTool()
{
}

ImporterSuite::~ImporterSuite()
{
}

int ImporterSuite::Execute(int argc, char ** argv)
{
	// nothing to do 
	return 0;
}

void ImporterSuite::Execute(const char * name)
{
#ifndef LIVE_RELEASE
	blender->SetScriptRoot(IMPORTER_PY_ROOT);
#endif

	blender->AddArgument("host", GetHost());
	if (name) {
		std::string path = TEST_ASSET_ROOT;
		path = path + "/" + name;
		blender->AddArgument("input", path);
	}

	Start();
	Join();
}
