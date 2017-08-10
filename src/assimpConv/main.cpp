/**
	A quick and dirty thool that loads, converts and saves models
	to the native format of assimp.

	@author Caiwan
*/

#include <stack>
#include <stdio.h>

#include <Windows.h>

#include "core/system.h"
#include "generator/SceneLoader.h"

#include "utils/logger.h"

#include "Arguments.hpp"
#include "assimploader.h"
#include "BlenderExportServer.h"

using namespace std;
using namespace ideup;

using namespace Grafkit;

typedef unsigned int uint;


/* ================================================================================================ */

class Application{
private:
	Arguments args;

public:
	Application() {
		args.add("blender", 'b').description("Run exporter script in blender").flag(true);
		args.add("help", 'h').description("Shows this help message.").flag(true);
		args.add("input", 'i').description("Input filename").required(true);
		args.add("output", 'o').description("Output filename").required(true);
		//args.add("format", 'f').description("Output format. Overrides file extension.");
		//args.add("axis", 'x').description("Change axis order of the vertex cordinate system and polarity. (like +x+y+z, +x-z+y, ... )");
		//args.add("flip", 'p').description("Flips the camera 180 deg around one given axis.");
		args.add("lh").description("convert to left handed").flag(true);
		//args.add("textures", 't').description("Strip path from texture filenames").flag(true);
		//args.add("animation", 'a').description("Merge animations from an external json file").flag(true);
		args.add("cwd").description("Command work directory, where the py folder is, if execution dir differs");
	}

	~Application() {
	}

public:

	int Execute(int argc, char* argv[]) {

		Log::Logger().AddHandler(new LoggerHandler::ConsoleLogger());
		Log::Logger().AddHandler(new LoggerHandler::FileLoggerHandler("log.log", "error.log"));

		if (!args.evaluate(argc, argv)) {
			Log::Logger().Info(args.getErrorMessage().c_str());
			Log::Logger().Info(args.getHelpMessage().c_str());
			return 1;
		} else if (args.get("help").isFound()) {
			Log::Logger().Info(args.getHelpMessage().c_str());
			return 0;
		}

		if (args.get("blender").isFound())
			return ExecuteBlender();
		
		return ExecuteAssimp();
	}

	int ExecuteBlender() {
		
		BlenderExportServer server;

		server.SetLHFlag(args.get("lh").isFound());
		//server.SetLHFlag(true);

		// check if input exists, otherwise do not start anything
		std::string fn = args.get("input").value();
		FILE* fp = nullptr; 
		if (fopen_s(&fp, fn.c_str(), "rb") != 0) {
			cout << "Could not open file:" << fn;
			return 1;
		} 
		fclose(fp);

		server.Start();

		std::string cwd = "";
		if (args.get("cwd").isFound()) {
			cwd = args.get("cwd").value();
			// add trailing slash if none
			if (cwd.at(cwd.length() - 1) != '/' || cwd.at(cwd.length() - 1) != '\\')
				cwd.append("/");
		}

		std::string cmd = "blender -b -P "+ cwd +"py/exporter.py --";
		
		cmd += " -i " + args.get("input").value();
		std::string host; server.GetHost(host);
		cmd += " -p " + host;

		std::array<char, 1024> buffer;
		
		//cmd = "{" + cmd + "}"
		cmd += " 2>&1";

		std::shared_ptr<FILE> pipe(_popen(cmd.c_str(), "r"), _pclose);	// http://stackoverflow.com/questions/35297970
		if (!pipe) {
			cout << "Could not start Blender. Args: " << cmd;
			return 1;
		}

		// pump 
		while (!feof(pipe.get())) {
			if (fgets(buffer.data(), buffer.size(), pipe.get()) != NULL) {
				//Log::Logger().Trace(buffer.data());
			}
		}

		server.Stop();

		SceneResRef scene = server.GetScene(); 
		if (scene.Valid() && scene->Valid())
			SceneLoader::Save(scene->Get(), args.get("output").value());

		return 0;
	}

	int ExecuteAssimp() {
		void *data = nullptr;
		size_t len = 0;

		FILE* fp = nullptr;
		errno_t err = fopen_s(&fp, args.get("input").value().c_str(), "rb");
		if (!err) {
			fseek(fp, 0, SEEK_END);
			len = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			data = malloc(len);
			fread_s(data, len, 1, len, fp);
			fclose(fp);

			try {
				AssimpLoader loader(data, len);
				SceneResRef scene = loader.Load();
				SceneLoader::Save(scene->Get(), args.get("output").value());
			}

			catch (FWdebug::Exception *ex) {
				cout << ex->what();
				delete ex;
			}
		}
		else {
			cout << "ERROR " << strerror(err) << endl;
			free(data);
			return 1;
		}

		free(data);

		return 0;
	}

};

/* ================================================================================================ */

//#define TESTING

// "-i ..\\..\\tests\\assets\\models\\test.blend -b -o test.scene"

int main(int argc, char* argv[]) {
	Application app;
	int ret = app.Execute(argc, argv);
	//system("pause");
	return ret;
}
