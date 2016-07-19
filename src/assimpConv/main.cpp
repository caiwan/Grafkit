/**
	A quick and dirtz thool that loads, converts and saves models 
	to the native format of assimp.

	@author Caiwan
*/

#include "Arguments.hpp"

#include "assimp/Importer.hpp"
#include "assimp/Exporter.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/matrix4x4.h"


using namespace std;
using namespace ideup;

typedef unsigned int uint;

aiTextureType textureTypes[] = {
	aiTextureType_DIFFUSE,
	aiTextureType_SPECULAR,
	aiTextureType_AMBIENT,
	aiTextureType_EMISSIVE,
	aiTextureType_HEIGHT,
	aiTextureType_NORMALS,
	aiTextureType_SHININESS,
	aiTextureType_OPACITY,
	aiTextureType_DISPLACEMENT,
	aiTextureType_LIGHTMAP,
	aiTextureType_REFLECTION
}; 

inline void swap_vertices(aiVector3D *vertices, char order[], char polarity[]) {
	aiVector3D v;
	v.x = (*vertices)[order[0]] * polarity[0];
	v.x = (*vertices)[order[1]] * polarity[1];
	v.x = (*vertices)[order[2]] * polarity[2];
	*vertices = v;
}

int run(int argc, char* argv[])
{
	Arguments args;

	args.add("help", 'h').description("Shows this help message.").flag(true);
	args.add("input", 'i').description("Input filename").required(true);
	args.add("output", 'o').description("Output filename").required(true);
	args.add("format", 'f').description("Output format. Overrides file extension.");
	args.add("axis").description("Change axis order of the cordinate system and polarity. (like +x+y+z, +x-z+y, ... )");
	args.add("textures").description("Strip path from texture filenames").flag(true);

	// parse args
	if (!args.evaluate(argc, argv)) {
		cout << args.getErrorMessage() << endl;
		return 1;
	}

	// print help
	if (argc == 1 || args.get("help").isFound()) {
		cout << args.getHelpMessage() << endl;
		return 0;
	}
 
	string inFileName = args.get("input").value();
	string outFileName = args.get("output").value();
	string outExtension;

	// determine file format
	if (!args.get("format").isFound())
	{
		string::size_type n;
		string s = outFileName;

		n = s.find_last_of('.');
		if (n != string::npos) {
			s = s.substr(n + 1);
		}
		if (s.empty()) {
			outExtension = "assbin";
			cout << "WARNING: No file extension was given. Using assbin format for default." << endl;
		}
		else {
			outExtension = s;
		}
	}
	else {
		outExtension = args.get("format").value();
	}

	// import scene 
	Assimp::Importer aiImporter;
	aiScene const * scene = aiImporter.ReadFile(inFileName.c_str(),
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_ConvertToLeftHanded |
		0);

	if (scene == nullptr) {
		cout << "Could not load model file" << inFileName << endl;
		cout << aiImporter.GetErrorString() << endl;
		return 1;
	}

	// flip axes
	if (args.get("axes").isFound() && scene->HasMeshes()) {
		string axesOrder = args.get("axes").value();
		if (axesOrder.length() != 6) {
			cout << args.getHelpMessage() << endl;
			return 1;
		}

		char order[3];
		char polarity[3];
		uint k = 3;
		while (k--) {
			char a = axesOrder.at(2*k), b = -1;
			char c = axesOrder.at(2*k+1), d = 1;
			switch (a) {
				case 'x': case 'X': b = 0; break;
				case 'y': case 'Y': b = 1; break;
				case 'z': case 'Z': b = 2; break;
				default:
					cout << args.getHelpMessage() << endl;
					return 1;
			}

			switch (c) {
				case '+': d = +1; break;
				case '-': d = -1; break;
			default:
				cout << args.getHelpMessage() << endl;
				return 1;
			}

			order[k] = b;
			polarity[k] = d;
		}

		for (uint i = 0; i < scene->mNumMeshes; i++) {
			aiMesh * const mesh = scene->mMeshes[i];
			for (uint j = 0; j < mesh->mNumVertices; j++) {
				swap_vertices(&mesh->mVertices[j], order, polarity);
				swap_vertices(&mesh->mNormals[j], order, polarity);
			}
		}

	}

	// strip texture filenames
	if (args.get("textures").isFound() && scene->HasMaterials()) {
		for (uint i = 0; i < scene->mNumMaterials; i++) {
			aiMaterial const * material = scene->mMaterials[i];
			for (uint j = 0; j < sizeof(textureTypes) / sizeof(textureTypes[0]); j++) {
				aiTextureType tt = textureTypes[j];
				for (uint k = 0; k < material->GetTextureCount(tt); k++) {
					aiString aiPath;
					material->GetTexture(tt, k, &aiPath);
					string path = aiPath.C_Str();
					{
						string s = path;
						string::size_type n;
						n = s.find_last_of('/');
						if (n != string::npos) {
							s  = s.substr(n + 1);
						}
						if (!s.empty()) {
							path = s;
						}
					}
					// textura filenev vissza
				}
			}
		}
	}


	// save 
	Assimp::Exporter aiExporter;

	if (aiExporter.Export(scene, outExtension, outFileName) != aiReturn_SUCCESS) {
		cout << "Could not save model file" << endl;
		cout << aiExporter.GetErrorString() << endl;
		return 1;
	}

	return 0;
}

int main(int argc, char* argv[]) {
	int result = run(argc, argv);
	return result;
}
