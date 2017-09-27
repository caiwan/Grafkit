#include "stdafx.h"

#include <string>
#include <stack>
#include <stdio.h>
#include <memory>

#include "BlenderThread.h"

#include "utils/logger.h"

using namespace Grafkit;
using namespace GKimporter;

#define DEFAULT_BUFLEN 16 * 4096

GKimporter::BlenderThread::BlenderThread() : isRunning(false), isTerminate(false)
{
	scriptRoot = "";
}

int GKimporter::BlenderThread::Run()
{
	std::array<char, DEFAULT_BUFLEN> buffer;

	// --- snip
	std::stringstream cmdArguments;

	cmdArguments << "blender ";
	cmdArguments << "-b -P " << scriptRoot << "/py/exporter.py" << " -- ";

	for (auto argument = arguments.begin(); argument != arguments.end(); ++argument) {
		cmdArguments << "-" << argument->first << " " << argument->second;
	}

	// --- snap
	
	const char * command = cmdArguments.str().c_str();
	std::shared_ptr<FILE> pipe(_popen(command, "r"), _pclose);
	if (!pipe) {
		Log::Logger().Error("Could not start Blender. Command: %s", command);
		return 1;
	}

	isRunning = true;
	
	// pump 
	while (!feof(pipe.get()) || !isTerminate) {
		if (fgets(buffer.data(), buffer.size(), pipe.get()) != NULL) {
			Log::Logger().Trace(buffer.data());
		}
	}

	isRunning = false;

	return 0;
}
