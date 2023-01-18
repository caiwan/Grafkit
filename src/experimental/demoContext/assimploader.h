#pragma once

#if 0

#include "Command.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/matrix4x4.h"

#include "assimp/vector2.h"
#include "assimp/vector3.h"
#include "assimp/quaternion.h"

#include "core/exceptions.h"

#include "core/asset.h"
#include "core/resource.h"
#include "resource/ResourceBuilder.h"

#include "render/Scene.h"
#include "render/model.h"
#include "render/texture.h"

struct aiNode;
struct aiMaterial;

namespace GKimporter {

	/*
		A huge dirty Importer class for the engine
	*/

	class AssimpLoader : public Command
	{
	public:
		AssimpLoader() {}
		~AssimpLoader(){ }

		virtual void Evaluate(Environment*& env, nlohmann::json json);
		virtual std::string GetCommandString() { return "collada"; }


	private:
		void AssimpLoadMaterials(Environment*& env);
		void AssimpLoadMeshes(Environment*& env);
		void AssimpLoadCameras(Environment*& env);
		void AssimpLoadLights(Environment*& env);
		void AssimpBuildScenegraph(Environment*& env);
		void AssimpLoadAnimations(Environment*& env);

		void AssimpParseScenegraphNode(Environment*& env, aiNode* ai_node, Grafkit::ActorRef &actor_node, int maxdepth = 1024);

		std::string GetMaterialName(int index);

	private:
		static Grafkit::Matrix ai4x4MatrixToFWMatrix(aiMatrix4x4 * m);
		static inline aiVector3D crossProduct(aiVector3D a, aiVector3D b);

		static inline float2 aiVector2DToFloat2(aiVector2D &v);
		static inline float2 aiVector3DToFloat2(aiVector3D &v);
		static inline float3 aiVector3DToFloat3(aiVector3D &v);
		static inline float4 aiVector3DToFloat4(aiVector3D &v, float w);
		static inline float4 aiQuaternionToFloat4(aiQuaternion &v);
		static inline float4 aiColor3DToFloat4(aiColor3D &c);

		static inline float4 aiMatkey4ToFloat4(aiMaterial *&mat, const char* key, int a1, int a2);
		static inline float aiMatkeyToFloat(aiMaterial *&mat, const char* key, int a1, int a2);

	private:
		Assimp::Importer importer;
		const aiScene *aiscene;
	};

}

// exceptions
DEFINE_EXCEPTION(AssimpParseException, 4000, "Error while parsing file via ASSIMP");

#endif
