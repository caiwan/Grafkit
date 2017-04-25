/**
	A quick and dirty thool that loads, converts and saves models
	to the native format of assimp.

	@author Caiwan
*/

#include<stack>

#include <Windows.h>

#include "core/system.h"
#include "generator/SceneLoader.h"

#include "Arguments.hpp"
#include "assimploader.h"

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
		args.add("help", 'h').description("Shows this help message.").flag(true);
		args.add("input", 'i').description("Input filename").required(true);
		args.add("output", 'o').description("Output filename").required(true);
		//args.add("format", 'f').description("Output format. Overrides file extension.");
		args.add("axis", 'x').description("Change axis order of the vertex cordinate system and polarity. (like +x+y+z, +x-z+y, ... )");
		args.add("flip", 'p').description("Flips the camera 180 deg around one given axis.");
		args.add("lh").description("convert to left handed").flag(true);
		args.add("textures", 't').description("Strip path from texture filenames").flag(true);
		args.add("animation", 'a').description("Merge animations from an external json file").flag(true);
	}

	~Application() {
	}

public:

	int execute(int argc, char* argv[]) {
		if (!args.evaluate(argc, argv)) {
			cout << args.getErrorMessage() << endl;
			cout << args.getHelpMessage() << endl;
			return 1;
		} else if (args.get("help").isFound()) {
			cout << args.getHelpMessage() << endl;
			return 0;
		}

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

int main(int argc, char* argv[]) {
	Application app;

#ifdef TESTING
	char *dummyargv[] = {
		argv[0],
		"-i", "..\\..\\tests\\assets\\models\\anim_complex_baked.dae",
		"-o", "anim_complex_baked.scene",
	};
	int k = app.execute(5, dummyargv);
	system("pause");
	return k;
#else
	return app.execute(argc, argv);
#endif

}
