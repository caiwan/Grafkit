/**
	A quick and dirty thool that loads, converts and saves models
	to the native format of assimp.

	@author Caiwan
*/

#include<stack>

#include <Windows.h>

#include "core/system.h"

#include "Arguments.hpp"

//#include "assimp/Importer.hpp"
//#include "assimp/Exporter.hpp"
//#include "assimp/scene.h"
//#include "assimp/postprocess.h"
//#include "assimp/material.h"
//#include "assimp/mesh.h"
//#include "assimp/matrix4x4.h"

//#include "utils/exceptions.h"
//
//#include "utils/asset.h"
//#include "utils/AssetFactory.h"
//#include "utils/AssetFile.h"
//
//#include "utils/ResourceManager.h"

#include "assimploader.h"


#include "generator/SceneLoader.h"

using namespace std;
using namespace ideup;

using namespace Grafkit;

typedef unsigned int uint;

//aiTextureType textureTypes[] = {
//	aiTextureType_DIFFUSE,
//	aiTextureType_SPECULAR,
//	aiTextureType_AMBIENT,
//	aiTextureType_EMISSIVE,
//	aiTextureType_HEIGHT,
//	aiTextureType_NORMALS,
//	aiTextureType_SHININESS,
//	aiTextureType_OPACITY,
//	aiTextureType_DISPLACEMENT,
//	aiTextureType_LIGHTMAP,
//	aiTextureType_REFLECTION
//};
//
//inline aiVector3D crossProduct(aiVector3D a, aiVector3D b) {
//	return aiVector3D(
//		(a.y*b.z - a.z*b.y),
//		(a.z*b.x - a.x*b.z),
//		(a.x*b.y - a.y*b.x)
//	);
//}
//
//inline void swap_vertices(aiVector3D *vertices, char order[], char polarity[]) {
//	aiVector3D v;
//	v.x = (*vertices)[order[0]] * polarity[0];
//	v.y = (*vertices)[order[1]] * polarity[1];
//	v.z = (*vertices)[order[2]] * polarity[2];
//	*vertices = v;
//}

/* ================================================================================================ */

class Application{
private:
	Arguments args;

public:
	Application() {
		args.add("help", 'h').description("Shows this help message.").flag(true);
		args.add("input", 'i').description("Input filename").required(true);
		args.add("output", 'o').description("Output filename").required(true);
		args.add("format", 'f').description("Output format. Overrides file extension.");
		args.add("axis", 'x').description("Change axis order of the vertex cordinate system and polarity. (like +x+y+z, +x-z+y, ... )");
		args.add("flip", 'p').description("Flips the camera 180 deg around one given axis.");
		args.add("lh").description("convert to left handed").flag(true);
		args.add("textures", 't').description("Strip path from texture filenames").flag(true);
	}

	~Application() {
	}

public:

	int execute(int argc, char* argv[]) {
		if (!args.evaluate(argc, argv)) {
			cout << args.getErrorMessage() << endl;
			cout << args.getHelpMessage() << endl;
			return 1;
		}

		void *data = nullptr;
		size_t len = 0;

		FILE* fp = nullptr;
		if (!fopen_s(&fp, args.get("input").value().c_str(), "rw")) {
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
			cout << ("PENIS PENIS PENIS");
			free(data);
			return 1;
		}

		free(data);

		return 0;
	}

};

/* ================================================================================================ */

int main(int argc, char* argv[]) {
	Application app;
	return app.execute(argc, argv);
}
