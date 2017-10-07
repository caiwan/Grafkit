#define _USE_MATH_DEFINES
#include <cmath>
#include <stack>
#include <vector>

#include "json.hpp"

#include "assimploader.h"

#include "assimp/Importer.hpp"
//#include "assimp/Exporter.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/matrix4x4.h"
#include "assimp/matrix3x3.h"

#include "stdafx.h" // az assimp sajat memoria basztatoja miatt kell lerakni ide, a sajat memoria basztatonk miatt

#include "math/matrix.h"
#include "math/quaternion.h"

#include "utils/resource.h"
#include "utils/ResourceManager.h"

#include "render/renderer.h"
#include "render/texture.h"
#include "render/Material.h"
#include "render/model.h"
#include "render/camera.h"
#include "render/light.h"
#include "render/animation.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

#define MAX_TEXCOORD_COUNT 2

namespace {
	// AI texture -- internal texture type mapping 
	const struct {
		enum aiTextureType ai;
		enum Material::texture_type_e tt;
	}
	texture_load_map[] = {
		aiTextureType_NONE,		    Material::TT_aux0,
		aiTextureType_DIFFUSE,      Material::TT_diffuse,
		aiTextureType_SPECULAR,     Material::TT_aux0,
		aiTextureType_AMBIENT,      Material::TT_aux0,
		aiTextureType_EMISSIVE,     Material::TT_aux0,
		aiTextureType_HEIGHT,       Material::TT_aux0,
		aiTextureType_NORMALS,      Material::TT_normal,
		aiTextureType_SHININESS,    Material::TT_aux0,
		aiTextureType_OPACITY,      Material::TT_alpha,
		aiTextureType_DISPLACEMENT, Material::TT_bump,
		aiTextureType_LIGHTMAP,		Material::TT_aux0,
		aiTextureType_REFLECTION,	Material::TT_aux0,
		aiTextureType_UNKNOWN,		Material::TT_aux0,
	};
}


// ================================================================================================================================================================
// It does the trick
// ================================================================================================================================================================

void GKimporter::AssimpLoader::Evaluate(Environment *& env, nlohmann::json json)
{
	std::string daeData = json;

	aiscene = importer.ReadFileFromMemory(daeData.c_str(), daeData.length(),
		aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace |
		0
	);

	if (!aiscene)
		throw new EX_DETAILS(AssimpParseException, importer.GetErrorString());

	AssimpLoadMaterials(env);
	AssimpLoadMeshes(env);
	AssimpLoadCameras(env);
	AssimpLoadLights(env);

	AssimpBuildScenegraph(env);

	AssimpLoadAnimations(env);

}

void GKimporter::AssimpLoader::AssimpLoadMaterials(Environment*& env)
{
	if (aiscene->HasMaterials())
	{
		LOGGER(Log::Logger().Trace("Materials"));
		for (size_t i = 0; i < aiscene->mNumMaterials; i++) {

			MaterialRef material = new Material();
			aiMaterial *srcMaterial = aiscene->mMaterials[i];

			material->SetName(GetMaterialName(i));

			LOGGER(Log::Logger().Trace("- #%d %s", i, material->GetName().c_str()));

			aiReturn texFound = AI_FAILURE;

			// --- SNIP ---
			// -- -- load textures
			// textura -> material 
			for (size_t k = 0; k < sizeof(texture_load_map) / sizeof(texture_load_map[0]); k++) {
				for (size_t j = 0; j < srcMaterial->GetTextureCount(texture_load_map[k].ai); j++) {
					LOGGER(Log::Logger().Trace("-- texture #%s #%d", texture_load_map[k].tt, j));
					// TODO: does it work at all ?
					// material->AddTexture(assimpTexture(texture_load_map[k].ai, curr_mat, j, resman), texture_load_map[k].tt);
				}
			}
			// --- SMAP ---


			// there is not much to set up atm
			material->Colors().diffuse = aiMatkey4ToFloat4(srcMaterial, AI_MATKEY_COLOR_DIFFUSE);
			material->Colors().specular = aiMatkey4ToFloat4(srcMaterial, AI_MATKEY_COLOR_SPECULAR);

			env->GetBuilder().AddMaterial(material);
		}
	}
}

void GKimporter::AssimpLoader::AssimpLoadMeshes(Environment*& env)
{
	if (aiscene->HasMeshes())
	{
		LOGGER(Log::Logger().Trace("Meshes"));

		for (size_t i = 0; i < aiscene->mNumMeshes; i++)
		{
			// -- meshes
			ModelRef model = new Model(new Mesh());
			aiMesh* srcMesh = aiscene->mMeshes[i];

			const char* meshName = srcMesh->mName.C_Str();
			LOGGER(Log::Logger().Trace("- #%d %s", i, meshName));
			model->SetName(meshName);

			float4* vertices = new float4[srcMesh->mNumVertices];

			float2** texuvs = new float2*[MAX_TEXCOORD_COUNT];
			for (size_t j = 0; j < MAX_TEXCOORD_COUNT; j++) {
				texuvs[j++] = new float2[srcMesh->mNumVertices];
			}
			float4* normals = new float4[srcMesh->mNumVertices];

			for (size_t k = 0; k < srcMesh->mNumVertices; k++) {
				vertices[k] = aiVector3DToFloat4(srcMesh->mVertices[k], 1.);
				normals[k] = aiVector3DToFloat4(srcMesh->mNormals[k], 0.);

				if (srcMesh->mTextureCoords && srcMesh->mTextureCoords[0]) {

					for (size_t j = 0; j < MAX_TEXCOORD_COUNT; j++) {
						aiVector3D *t = srcMesh->mTextureCoords[j];
						if (t)
							texuvs[j][k] = aiVector3DToFloat2(t[k]);
						else
							texuvs[j][k] = float2(0, 0);
					}


				}

				model->GetMesh()->AddPointer("POSITION", srcMesh->mNumVertices * sizeof(*vertices), &vertices[0]);
				model->GetMesh()->AddPointer("NORMAL", srcMesh->mNumVertices * sizeof(*normals), &normals[0]);

				//if (srcMesh->mTextureCoords && srcMesh->mTextureCoords[0]) {
				for (size_t j = 0; j < MAX_TEXCOORD_COUNT; j++) {
					std::string buf = "TEXCOORD" + std::to_string(i);
					model->GetMesh()->AddPointer(buf, srcMesh->mNumVertices * sizeof(**texuvs), &texuvs[j]);
				}
			}

			// -- faces

			if (srcMesh->HasFaces()) {
				std::vector<int> indices;

				for (size_t j = 0; j < srcMesh->mNumFaces; j++)
				{
					aiFace *curr_face = &srcMesh->mFaces[j];
					for (size_t k = 0; k < curr_face->mNumIndices; k++) {
						indices.push_back(curr_face->mIndices[k]);
					}
				}

				model->GetMesh()->SetIndices(srcMesh->mNumVertices, indices.size(), &indices[0]);
			}
			else {
				///@todo arra az esetre is kell valamit kitalalni, amikor nincsenek facek, csak vertexek.
			}

			env->GetBuilder().AddModel(model);

			MaterialRef material = env->GetBuilder().FindMaterial(srcMesh->mMaterialIndex);
			model->SetMaterial(material);
		}
	}
}

void GKimporter::AssimpLoader::AssimpLoadCameras(Environment*& env)
{
	if (aiscene->HasCameras()) {
		LOGGER(Log::Logger().Trace("Cameras"));
		for (size_t i = 0; i < aiscene->mNumCameras; i++) {

			aiCamera *curr_camera = aiscene->mCameras[i];

			CameraRef camera = new Camera(Camera::CAMERA_LH);

			const char* camera_name = curr_camera->mName.C_Str();
			LOGGER(Log::Logger().Trace("- #%d %s", i, camera_name));

			camera->SetName(camera_name);

			camera->SetFOV(curr_camera->mHorizontalFOV);
			camera->SetClippingPlanes(curr_camera->mClipPlaneNear, curr_camera->mClipPlaneFar);

			camera->SetAspect(curr_camera->mAspect);

			env->GetBuilder().AddCamera(camera);

		}
	}
}

void GKimporter::AssimpLoader::AssimpLoadLights(Environment*& env)
{
	if (aiscene->HasLights()) {
		LOGGER(Log::Logger().Trace("Lights"));
		for (size_t i = 0; i < aiscene->mNumLights; i++) {
			LightRef light;
			aiLight *srcLight = aiscene->mLights[i];

			const char *light_name = srcLight->mName.C_Str();
			LOGGER(Log::Logger().Trace("- #%d %s %d", i, light_name, srcLight->mType));



			switch (srcLight->mType) {

				// we support point light atm

			case aiLightSource_POINT:
				light = new Light(Light::LT_point);
				light->Position() = aiVector3DToFloat4(srcLight->mPosition, 1.);
				break;

#if 0
			case aiLightSource_DIRECTIONAL:
				light = new Light(Light::LT_directional);
				ASSIMP_V3D_F4(srcLight->mDirection, light->Direction(), 0, 0f);
				break;

			case aiLightSource_SPOT:
				light = new Light(Light::LT_spot);
				ASSIMP_V3D_F4(srcLight->mPosition, light->Position(), 1.0f);
				ASSIMP_V3D_F4(srcLight->mDirection, light->Direction(), 0.0f);
				(light->Angle()) = srcLight->mAngleInnerCone;
				(light->Falloff()) = srcLight->mAngleOuterCone - srcLight->mAngleInnerCone;
				break;

			case aiLightSource_AMBIENT:
				light = new Light(Light::LT_ambient);
				break;
#endif
			}

			if (light.Valid()) {
				light->SetName(light_name);

				(light->ConstantAttenuation()) = srcLight->mAttenuationConstant;
				(light->LinearAttenuation()) = srcLight->mAttenuationLinear;
				(light->QuardicAttenuation()) = srcLight->mAttenuationQuadratic;

				light->Diffuse() = aiColor3DToFloat4(srcLight->mColorDiffuse);

				env->GetBuilder().AddLight(light);
			}
		}
	}
}

void GKimporter::AssimpLoader::AssimpBuildScenegraph(Environment*& env)
{
	ActorRef root_node = new Actor;
	LOGGER(Log::Logger().Trace("Building scenegraph"));
	AssimpParseScenegraphNode(env, aiscene->mRootNode, root_node);

	env->GetBuilder().SetRootActor(root_node);

	// --- kamera helyenek kiszedese a scenegraphbol
	if (aiscene->HasCameras()) {
		for (size_t i = 0; i < aiscene->mNumCameras; i++) {
			aiCamera *curr_camera = aiscene->mCameras[i];
			std::string name = curr_camera->mName.C_Str();

			CameraRef camera = env->GetBuilder().FindCamera(name);
			ActorRef actor = env->GetBuilder().FindActor(name);

			actor->AddEntity(camera);

		}
	}

	// fenyek helyenek kiszedese a scenegraphbol
	if (aiscene->HasLights()) {
		for (size_t i = 0; i < aiscene->mNumLights; i++) {
			aiLight * curr_light = aiscene->mLights[i];

			std::string name = curr_light->mName.C_Str();

			LightRef light = env->GetBuilder().FindLight(name);
			ActorRef actor = env->GetBuilder().FindActor(name);

			actor->AddEntity(light);
		}
	}

}

void GKimporter::AssimpLoader::AssimpLoadAnimations(Environment*& env)
{
#if 0
	size_t i = 0, j = 0, k = 0, l = 0;
	// --- Animacio kiszedese
	if (aiscene->HasAnimations()) {
		LOGGER(Log::Logger().Trace("Animation"));
		for (i = 0; i < aiscene->mNumAnimations; i++) {
			aiAnimation *curr_anim = aiscene->mAnimations[i];
			aiString name = curr_anim->mName;

			LOGGER(Log::Logger().Trace("- #%d : %s", i, name.C_Str()));

			for (j = 0; j < curr_anim->mNumChannels; j++) {
				aiNodeAnim * curr_nodeAnim = curr_anim->mChannels[j];

				LOGGER(Log::Logger().Trace("-- #%d : %s", j, curr_nodeAnim->mNodeName.C_Str()));

				auto it = m_resources.actors.find(curr_nodeAnim->mNodeName.C_Str());
				if (it != m_resources.actors.end()) {
					ActorAnimation* anim = new ActorAnimation();
					anim->SetActor(it->second);

					float3 f3;
					float4 f4;

					for (k = 0; k < curr_nodeAnim->mNumPositionKeys; k++) {
						ASSIMP_V3D_F3(curr_nodeAnim->mPositionKeys[k].mValue, f3);
						anim->AddPositionKey(curr_nodeAnim->mPositionKeys[k].mTime, f3);
					}

					for (k = 0; k < curr_nodeAnim->mNumScalingKeys; k++) {
						ASSIMP_V3D_F3(curr_nodeAnim->mScalingKeys[k].mValue, f3);
						anim->AddScalingKey(curr_nodeAnim->mScalingKeys[k].mTime, f3);
					}

					for (k = 0; k < curr_nodeAnim->mNumRotationKeys; k++) {
						ASSIMP_V4D_F4(curr_nodeAnim->mRotationKeys[k].mValue, f4);
						float4 lolmi = Quaternion(f4).toAxisAngle(); //dbg
						anim->AddRotationKey(curr_nodeAnim->mRotationKeys[k].mTime, f4);
					}

					anim->SetDuration(curr_anim->mDuration);
					anim->SetName(curr_nodeAnim->mNodeName.C_Str());

					outScene->AddAnimation(anim);
				}
			}

			/* (...) */
		}
	}
#endif
}

// ================================================================================================================================================================
// 
// ================================================================================================================================================================

void GKimporter::AssimpLoader::AssimpParseScenegraphNode(Environment*& env, aiNode * ai_node, ActorRef & actor_node, int maxdepth)
{
	if (!ai_node)
		return;

	if (maxdepth < 0)
		return;

	ActorRef actor = new Actor();

	const char * name = ai_node->mName.C_Str();
	LOGGER(
		int depth = 1024 - maxdepth; char tab[1024]; tab[depth] = 0;
	while (depth--) tab[depth] = ' ';

	Log::Logger().Trace(" %s%s [%d]", tab, name, ai_node->mNumMeshes);

	size_t buflen = strlen(tab) + 8 * ai_node->mNumMeshes + 256;
	char kbuf[4096];
	sprintf_s(kbuf, 4096, "%s meshes:", tab);
	);

	actor->SetName(name);

	// ---
	for (size_t i = 0; i < ai_node->mNumMeshes; i++) {
		UINT mesh_id = ai_node->mMeshes[i];

		ModelRef model = env->GetBuilder().FindModel(mesh_id);
		actor->AddEntity(model);

		LOGGER(sprintf_s(kbuf, buflen, "%s %d,", kbuf, i));
	}

	LOGGER(if (ai_node->mNumMeshes) Log::Logger().Trace(" %s", kbuf));

	actor->SetName(ai_node->mName.C_Str());
	actor->Matrix().Set(ai4x4MatrixToFWMatrix(&ai_node->mTransformation));

	env->GetBuilder().AddActor(actor);

	// next nodes
	for (size_t i = 0; i < ai_node->mNumChildren; i++) {
		ActorRef child = nullptr;
		AssimpParseScenegraphNode(env, ai_node->mChildren[i], child, maxdepth - 1);
		actor->AddChild(child);
	}

	actor_node = actor;
}

std::string GKimporter::AssimpLoader::GetMaterialName(int index)
{
	if (index < aiscene->mNumMaterials) {
		aiString name;
		aiMaterial *srcMaterial = aiscene->mMaterials[index];

		if (srcMaterial->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
			return std::string(name.C_Str());
		}
		else {
			char buff[256];
			sprintf_s(buff, "material%d", index);
			return std::string(buff);
		}
	}

	return std::string();
}

// ================================================================================================================================================================
// 
// ================================================================================================================================================================


inline Grafkit::Matrix GKimporter::AssimpLoader::ai4x4MatrixToFWMatrix(aiMatrix4x4 * m)
{
	if (!m)
		throw new EX(NullPointerException);

	return Grafkit::Matrix(
		m->a1, m->b1, m->c1, m->d1,
		m->a2, m->b2, m->c2, m->d2,
		m->a3, m->b3, m->c3, m->d3,
		m->a4, m->b4, m->c4, m->d4
	);
}

// This is another mess I'd like to remove or at least do something with it



/**
*/
inline aiVector3D GKimporter::AssimpLoader::crossProduct(aiVector3D a, aiVector3D b) {
	return aiVector3D(
		(a.y*b.z - a.z*b.y),
		(a.z*b.x - a.x*b.z),
		(a.x*b.y - a.y*b.x)
	);
}

inline float2 GKimporter::AssimpLoader::aiVector2DToFloat2(aiVector2D & v)
{
	return float2(v.x, v.y);
}

inline float2 GKimporter::AssimpLoader::aiVector3DToFloat2(aiVector3D & v)
{
	return float2(v.x, v.y);
}

inline float3 GKimporter::AssimpLoader::aiVector3DToFloat3(aiVector3D &v)
{
	return float3(v.x, v.y, v.z);
}

inline float4 GKimporter::AssimpLoader::aiVector3DToFloat4(aiVector3D &v, float w)
{
	return float4(v.x, v.y, v.z, w);
}

inline float4 GKimporter::AssimpLoader::aiColor3DToFloat4(aiColor3D &c)
{
	return float4(c.r, c.g, c.b, 1.);
}

inline float4 GKimporter::AssimpLoader::aiMatkey4ToFloat4(aiMaterial *& mat, const char * key, int a1, int a2)
{
	aiColor4D ac;
	mat->Get(key, a1, a2, ac);
	return float4(ac.r, ac.g, ac.g, ac.a);
}

inline float GKimporter::AssimpLoader::aiMatkeyToFloat(aiMaterial *& mat, const char * key, int a1, int a2)
{
	float f = 0.;
	mat->Get(key, a1, a2, f);
	return 0.0f;
}
