#pragma once 

#include "core/thread.h"

namespace GKimporter {
	/* 
	This runs Blender process on its own thread separately
	*/
	class BlenderThread : public Grafkit::Thread {
	public:
		BlenderThread();

		void AddArgument(std::string argument, std::string value) { arguments[argument] = value; }
		void SetScriptRoot(std::string root) { scriptRoot = root; }

		void Terminate() { isTerminate = true; }

		bool IsRunning() { return isRunning; }

	protected:
		virtual int Run();

		std::string GetExecuteCommand();

	private:
		std::map<std::string, std::string> arguments;

		std::string scriptRoot;

		bool isRunning;
		bool isTerminate;

	};
}