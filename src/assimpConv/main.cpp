/**
	A quick and dirtz thool that loads, converts and saves models 
	to the native format of assimp.

	@author Caiwan
*/

#include<stack>

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
	v.y = (*vertices)[order[1]] * polarity[1];
	v.z = (*vertices)[order[2]] * polarity[2];
	*vertices = v;
}

inline aiMatrix4x4 swap_matrix_columns(aiMatrix4x4 matrix, char order[], char polarity[]) {
	aiMatrix4x4 m;
	for (int i = 0; i < 4; i++) {
#if 0
		m.m[i][0] = matrix.m[i][order[0]] * polarity[0];
		m.m[i][1] = matrix.m[i][order[1]] * polarity[1];
		m.m[i][2] = matrix.m[i][order[2]] * polarity[2];
		m.m[i][3] = matrix.m[i][3];
#else
		m.m[0][i] = matrix.m[order[0]][i] * polarity[0];
		m.m[1][i] = matrix.m[order[1]][i] * polarity[1];
		m.m[2][i] = matrix.m[order[2]][i] * polarity[2];
		m.m[3][i] = matrix.m[3][i];
#endif
	}
	return m;
}

int run(int argc, char* argv[])
{
	Arguments args;

	args.add("help", 'h').description("Shows this help message.").flag(true);
	args.add("input", 'i').description("Input filename").required(true);
	args.add("output", 'o').description("Output filename").required(true);
	args.add("format", 'f').description("Output format. Overrides file extension.");
	args.add("axis", 'x').description("Change axis order of the vertex cordinate system and polarity. (like +x+y+z, +x-z+y, ... )");
	args.add("up", 'u').description("shanges the up vector of all the cameras (like +y, -z ... )");
	args.add("textures", 't').description("Strip path from texture filenames").flag(true);


	Assimp::Importer aiImporter;
	Assimp::Exporter aiExporter;

	// parse args
	if (!args.evaluate(argc, argv)) {
		cout << args.getErrorMessage() << endl;
		cout << args.getHelpMessage() << endl;
		return 1;
	}

	// print help
	if (argc == 1 || args.get("help").isFound()) {
		cout << args.getHelpMessage() << endl;
		// +++ supported file formats
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
	aiScene const * scene = aiImporter.ReadFile(inFileName.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		// aiProcessPreset_TargetRealtime_Quality |
		// aiProcess_ConvertToLeftHanded |
		0);

	if (scene == nullptr) {
		cout << "Could not load model file" << inFileName << endl;
		cout << aiImporter.GetErrorString() << endl;
		return 1;
	}

	// flip axes
	if (args.get("axis").isFound() && scene->HasMeshes()) {
		string axesOrder = args.get("axis").value();
		if (axesOrder.length() != 6) {
			cout << args.getHelpMessage() << endl;
			return 1;
		}

		char order[3];
		char polarity[3];
		uint k = 3;
		while (k--) {
			char c = axesOrder.at(2*k), b = -1;
			char a = axesOrder.at(2*k+1), d = 1;
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
		
		// reorder vertices
		
#if 0
		for (uint i = 0; i < scene->mNumMeshes; i++) {
			aiMesh * const mesh = scene->mMeshes[i];
			for (uint j = 0; j < mesh->mNumVertices; j++) {
				swap_vertices(&mesh->mVertices[j], order, polarity);
				swap_vertices(&mesh->mNormals[j], order, polarity);
			}
		}
#endif // 0		
		// reorder matrices
#if 0
		map<string, aiMatrix4x4> matrixMap;
		map<string, aiNode*> nodeMap;
		if (scene->mRootNode && scene->mRootNode->mNumChildren)
		{
			bool done = false;
			stack<aiNode*> stack;
			
			stack.push(scene->mRootNode);

			while (!stack.empty()) {
				aiNode *node = stack.top(); stack.pop();

				// yield current node
				if (node) {
					// store node and old matrix
					string name = node->mName.C_Str();
					matrixMap[name] = node->mTransformation;
					nodeMap[name] = node;

					node->mTransformation = swap_matrix_columns(node->mTransformation, order, polarity);
				}
				
				// push next
				for (uint i=0; i<node->mNumChildren; i++){
					stack.push(node->mChildren[i]);
				}
			}
		}
#endif // 0

#if 0
		// reorder cameras
		if (scene->HasCameras()) {
			for (uint i = 0; i < scene->mNumCameras; i++) {
				
				// reset camera matrices
				aiCamera *camera = scene->mCameras[i];
				string name = camera->mName.C_Str();
				auto itMatrix = matrixMap.find(name);
				auto itNode = nodeMap.find(name);
				
				if (itMatrix != matrixMap.end() && itNode != nodeMap.end())
					itNode->second->mTransformation = itMatrix->second;

			}
		}
#endif 

#if 0
		// reorder animations
		if (scene->HasAnimations()) {
			for (uint i = 0; i < scene->mNumAnimations; i++) {
				aiAnimation* anim = scene->mAnimations[i];
				// ... reverse engineer
				cout << anim->mName.C_Str() << endl;
			}
		}
#endif //0

	}

	// shange the up vector
	if (args.get("up").isFound() && scene->HasCameras()) {
		// string cameraUp = args.get("up").value();

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
						string::size_type n, m;
						n = s.find_last_of('/');
						m = s.find_last_of('\\');
						if (n != string::npos) {
							s  = s.substr(n + 1);
						}
						else if (m != string::npos) {
							s = s.substr(m + 1);
						}

						if (!s.empty()) {
							path = s;
						}
					}

					// textura filenev vissza
					const aiMaterialProperty* pp = nullptr;
					if (aiGetMaterialProperty(material, AI_MATKEY_TEXTURE(tt, k), &pp) == AI_SUCCESS) {
						aiMaterialProperty* pProp = const_cast<aiMaterialProperty*>(pp);
						if (aiPTI_String == pProp->mType) {
							size_t newLen = path.length() + 5;
							char* newData = (char*)malloc(newLen);
							(*(uint*)(&newData[0])) = path.length();
							memcpy_s(&newData[4], path.length()+1, path.c_str(), path.length()+1);
							free(pProp->mData);
							pProp->mData = newData;
							pProp->mDataLength = newLen;
						}
					}
				}
			}
		}
	}

	// save 

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
