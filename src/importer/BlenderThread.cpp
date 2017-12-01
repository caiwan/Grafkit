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

GKimporter::BlenderThread::BlenderThread() : Grafkit::Thread(), isRunning(false), isTerminate(false)
{
	scriptRoot = "";
}

int GKimporter::BlenderThread::Run()
{
	std::array<char, DEFAULT_BUFLEN> buffer;

	isRunning = true;

	std::string execCommand = GetExecuteCommand();

	const char * command = execCommand.c_str();
	
	Log::Logger().Info("Starting Blender: %s", command);

	std::shared_ptr<FILE> pipe(_popen(command, "r"), _pclose);
	if (!pipe) {
		Log::Logger().Error("Could not start Blender. Command: %s", command);
		return 1;
	}

	// pump stdout
	while (!feof(pipe.get()) && !isTerminate) {
		if (fgets(buffer.data(), buffer.size(), pipe.get()) != NULL) {
			Log::Logger().Trace(buffer.data());
		}
	}

	isRunning = false;

	return 0;
}

std::string GKimporter::BlenderThread::GetExecuteCommand()
{
	std::stringstream cmdLine;

	cmdLine << "blender ";
	cmdLine << "-b -P " << scriptRoot << "/py/exporter.py" << " -- ";

	for (auto argument = arguments.begin(); argument != arguments.end(); ++argument) {
		cmdLine << "--" << argument->first << " " << argument->second << " ";
	}


	cmdLine.put(0);
	cmdLine.seekp(0);

	return cmdLine.str();
}
