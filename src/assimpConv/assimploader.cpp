#define _USE_MATH_DEFINES
#include <cmath>
#include <stack>
#include <vector>

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

namespace {
	// AI texture -- internal texture type mapping 
	const struct {
		enum aiTextureType ai;
		enum Material::texture_type_e tt;
	}
	texture_load_map[] = {
		aiTextureType_NONE,		    Material::TT_aux0,
		aiTextureType_DIFFUSE,      Material::TT_diffuse,
		aiTextureType_SPECULAR,     Material::TT_specular,
		aiTextureType_AMBIENT,      Material::TT_aux0,
		aiTextureType_EMISSIVE,     Material::TT_aux0,
		aiTextureType_HEIGHT,       Material::TT_aux0,
		aiTextureType_NORMALS,      Material::TT_normal,
		aiTextureType_SHININESS,    Material::TT_shiniess,
		aiTextureType_OPACITY,      Material::TT_alpha,
		aiTextureType_DISPLACEMENT, Material::TT_bump,
		aiTextureType_LIGHTMAP,		Material::TT_aux0,
		aiTextureType_REFLECTION,	Material::TT_reflect,
		aiTextureType_UNKNOWN,		Material::TT_aux0,
	};
}


// ================================================================================================================================================================
// Head
// ================================================================================================================================================================

Grafkit::AssimpLoader::AssimpLoader() : m_data(nullptr), m_length(0),
m_is_lh(false)
{
}

Grafkit::AssimpLoader::AssimpLoader(void * src_data, size_t src_length) : m_data(src_data), m_length(src_length),
m_is_lh(false)
{
}


Grafkit::AssimpLoader::~AssimpLoader()
{
}

void Grafkit::AssimpLoader::SetLHFlag(bool islh)
{
	m_is_lh = islh;
}


// ================================================================================================================================================================
// It does the trick
// ================================================================================================================================================================
SceneResRef Grafkit::AssimpLoader::Load()
{
	if (!m_data)
		throw EX_DETAILS(AssimpParseException, "Nem tudom betolteni a forras assetet");

	SceneRef outScene = new Scene();

	AppendAssimp(m_data, m_length, outScene);

	return new Resource<Scene>(outScene);
}

void Grafkit::AssimpLoader::AppendAssimp(const void * data, size_t length, SceneRef inScene)
{
	/// @todo genNormals szar. Miert?
	aiscene = importer.ReadFileFromMemory(data, length,
		(m_is_lh ? aiProcess_ConvertToLeftHanded : 0) |
		0
	);

	if (!aiscene)
		throw EX_DETAILS(AssimpParseException, importer.GetErrorString());

	AssimpLoadMaterials(inScene);
	AssimpLoadMeshes(inScene);
	AssimpLoadCameras(inScene);
	AssimpLoadLights(inScene);

	AssimpBuildScenegraph(inScene);

	AssimpLoadAnimations(inScene);
}

void Grafkit::AssimpLoader::AssimpLoadMaterials(SceneRef &outScene)
{
	size_t i = 0, j = 0, k = 0, l = 0;
	std::vector<MaterialRef> &materials = m_resources.materials;

	if (aiscene->HasMaterials())
	{
		LOGGER(Log::Logger().Trace("Materials"));
		for (i = 0; i<aiscene->mNumMaterials; i++) {
			aiString path, name;

			/// @todo milyen material kell 
			MaterialRef material = new Material();
			aiMaterial *curr_mat = aiscene->mMaterials[i];

			///@todo nevekre szuksegunk ven-e?
			if (curr_mat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
				material->SetName(name.C_Str());
			}
			else {
				char buff[256];
				sprintf_s(buff, "material%d", i);
				material->SetName(buff);
			}

			LOGGER(Log::Logger().Trace("- #%d %s", i, material->GetName().c_str()));

			j = 0;
			aiReturn texFound = AI_FAILURE;

			// -- -- load textures
			// textura -> material 
			for (k = 0; k < sizeof(texture_load_map) / sizeof(texture_load_map[0]); k++) {
				for (j = 0; j < curr_mat->GetTextureCount(texture_load_map[k].ai); j++) {
					LOGGER(Log::Logger().Trace("-- texture #%s #%d", texture_load_map[k].tt, j));
					// material->AddTexture(assimpTexture(texture_load_map[k].ai, curr_mat, j, resman), texture_load_map[k].tt);
				}
			}

			ASSIMPMATERIALKEY_F4(curr_mat, AI_MATKEY_COLOR_DIFFUSE, material->GetDiffuse());
			ASSIMPMATERIALKEY_F4(curr_mat, AI_MATKEY_COLOR_AMBIENT, material->GetAmbient());
			ASSIMPMATERIALKEY_F4(curr_mat, AI_MATKEY_COLOR_SPECULAR, material->GetSpecular());

			ASSIMPMATERIALKEY_FLOAT(curr_mat, AI_MATKEY_SHININESS, material->GetShininess());
			ASSIMPMATERIALKEY_FLOAT(curr_mat, AI_MATKEY_SHININESS_STRENGTH, material->GetSpecularLevel());

			materials.push_back(material);
			///@todo itt a resource managert kellene hasznalni
		}
	}
}

void Grafkit::AssimpLoader::AssimpLoadMeshes(SceneRef &outScene)
{
	size_t i = 0, j = 0, k = 0, l = 0;
	
	std::vector<ModelRef> &models = m_resources.models;
	std::vector<MaterialRef> &materials = m_resources.materials;

	if (aiscene->HasMeshes())
	{
		LOGGER(Log::Logger().Trace("Meshes"));

		for (i = 0; i<aiscene->mNumMeshes; i++)
		{
			// -- meshes
			ModelRef model = new Model(new Mesh());
			aiMesh* curr_mesh = aiscene->mMeshes[i];

			const char* mesh_name = curr_mesh->mName.C_Str(); //for dbg purposes
			LOGGER(Log::Logger().Trace("- #%d %s", i, mesh_name));
			model->SetName(mesh_name);

			float4* vertices = new float4[curr_mesh->mNumVertices];
			float2* texuvs   = new float2[curr_mesh->mNumVertices];
			float4* normals  = new float4[curr_mesh->mNumVertices];
			float4* tangents = new float4[curr_mesh->mNumVertices];

			for (k = 0; k < curr_mesh->mNumVertices; k++) {
				float4 v;
				assimp_vertices(curr_mesh->mVertices[k], v, 1.); vertices[k] = v;
				assimp_vertices(curr_mesh->mNormals[k], v, 1.); normals[k] = v;

				if (curr_mesh->mTextureCoords && curr_mesh->mTextureCoords[0]) {
					// @todo valamiert nincs mindig tangent
					if (curr_mesh->mTangents) {
						assimp_vertices(curr_mesh->mTangents[k], v, 1.); tangents[k] = v;
					}
					assimp_vertices(curr_mesh->mTextureCoords[0][k], v, 1.); texuvs[k]= (float2(v.x, v.y));
				}

			}

			/**/

			model->GetMesh()->AddPointer("POSITION", curr_mesh->mNumVertices * sizeof(*vertices), &vertices[0]);
			model->GetMesh()->AddPointer("NORMAL", curr_mesh->mNumVertices * sizeof(*normals), &normals[0]);

			if (curr_mesh->mTextureCoords && curr_mesh->mTextureCoords[0]) {
				model->GetMesh()->AddPointer("TEXCOORD", curr_mesh->mNumVertices * sizeof(*texuvs), &texuvs[0]);
				if (curr_mesh->mTangents)
					model->GetMesh()->AddPointer("TANGENT", curr_mesh->mNumVertices * sizeof(*tangents), &tangents[0]);
			}

			// -- faces

			///@todo az indexek gyujteset lehessen kulturaltabb modon is vegezni valahogy
			if (curr_mesh->HasFaces()) {
				std::vector<int> indices;

				for (j = 0; j < curr_mesh->mNumFaces; j++)
				{
					aiFace *curr_face = &curr_mesh->mFaces[j];
					for (k = 0; k < curr_face->mNumIndices; k++) {
						indices.push_back(curr_face->mIndices[k]);
					}
				}

				model->GetMesh()->SetIndices(curr_mesh->mNumVertices, indices.size(), &indices[0]);
			}
			else {
				///@todo arra az esetre is kell valamit kitalalni, amikor nincsenek facek, csak vertexek.
			}

			// -- lookup materials
			int mat_index = curr_mesh->mMaterialIndex;
			if (materials.size()>mat_index)
			{
				model->SetMaterial(materials[mat_index]);
			}

			models.push_back(model);
			///@todo itt a resource managert kellene hasznalni
		}
	}
}

void Grafkit::AssimpLoader::AssimpLoadCameras(SceneRef &outScene)
{
	size_t i = 0, j = 0, k = 0, l = 0;
	std::vector<CameraRef> &cameras = m_resources.cameras;
	if (aiscene->HasCameras()) {
		LOGGER(Log::Logger().Trace("Cameras"));
		for (i = 0; i < aiscene->mNumCameras; i++) {

			aiCamera *curr_camera = aiscene->mCameras[i];

			/// TODO az assimp nem kezeli kulon a perspektiv kamerat
			CameraRef camera = new Camera();

			const char* camera_name = curr_camera->mName.C_Str();
			LOGGER(Log::Logger().Trace("- #%d %s", i, camera_name));

			camera->SetName(camera_name);

			camera->SetFOV(curr_camera->mHorizontalFOV);
			camera->SetClippingPlanes(curr_camera->mClipPlaneNear, curr_camera->mClipPlaneFar);

			// camera <- assmimp camera

			ASSIMP_V3D_F3_SET(curr_camera->mPosition, camera->SetPosition);
			ASSIMP_V3D_F3_SET(curr_camera->mLookAt, camera->SetLookTo); // aiLookat == lookTowardsVector
			ASSIMP_V3D_F3_SET(curr_camera->mUp, camera->SetUp);

			// camera->SetUp(0, -1, 0);

			// itt van meg aspekt, amivel kezdeni lehetne valamit

			cameras.push_back(camera);
		}
	}
}

void Grafkit::AssimpLoader::AssimpLoadLights(SceneRef &outScene)
{
	size_t i = 0, j = 0, k = 0, l = 0;
	std::vector<LightRef> &lights = m_resources.lights;
#if 1
	// load lights
	if (aiscene->HasLights()) {
		LOGGER(Log::Logger().Trace("Lights"));
		for (i = 0; i < aiscene->mNumLights; i++) {
			LightRef light;
			aiLight *curr_light = aiscene->mLights[i];

			const char *light_name = curr_light->mName.C_Str();
			LOGGER(Log::Logger().Trace("- #%d %s %d", i, light_name, curr_light->mType));


			// light <- assimp light
			switch (curr_light->mType) {
			case aiLightSource_DIRECTIONAL:
				light = new DirectionalLight();
				ASSIMP_V3D_F4(curr_light->mDirection, light->Direction(), 0, 0f);
				break;

			case aiLightSource_POINT:
				light = new PointLight();
				ASSIMP_V3D_F4(curr_light->mPosition, light->Position(), 0.0f);
				break;

			case aiLightSource_SPOT:
				light = new SpotLight();
				ASSIMP_V3D_F4(curr_light->mPosition, light->Position(), 1.0f);
				ASSIMP_V3D_F4(curr_light->mDirection, light->Direction(), 0.0f);
				(light->Angle()) = curr_light->mAngleInnerCone;
				(light->Falloff()) = curr_light->mAngleOuterCone - curr_light->mAngleInnerCone;
				break;

			case aiLightSource_AMBIENT:
				light = new AmbientLight();
				break;
			}

			(light->ConstantAttenuation()) = curr_light->mAttenuationConstant;
			(light->LinearAttenuation()) = curr_light->mAttenuationLinear;
			(light->QuardicAttenuation()) = curr_light->mAttenuationQuadratic;

			ASSIMP_COLOR_F4(curr_light->mColorAmbient, light->Ambient());
			ASSIMP_COLOR_F4(curr_light->mColorDiffuse, light->Diffuse());
			ASSIMP_COLOR_F4(curr_light->mColorSpecular, light->Specular());

			///@todo ez kell-e majd?
			light->SetName(light_name);

			lights.push_back(light);
		}
	}

#endif
}

void Grafkit::AssimpLoader::AssimpBuildScenegraph(SceneRef &outScene)
{
	size_t i = 0, j = 0, k = 0, l = 0;
	// build up scenegraph

	ActorRef root_node = new Actor;
	LOGGER(Log::Logger().Trace("Building scenegraph"));
	AssimpParseScenegraphNode(aiscene->mRootNode, root_node);
	outScene->Initialize(root_node);

	/* Workaround:
	* a root node matrixat identbe kell tenni, mert vices dolgokat csinal
	*/

	// root_node->Matrix().Identity();

	// --- kamera helyenek kiszedese a scenegraphbol
	if (aiscene->HasCameras()) {
		for (i = 0; i < aiscene->mNumCameras; i++) {
			aiCamera *curr_camera = aiscene->mCameras[i];
			std::string name = curr_camera->mName.C_Str();

			auto it = m_resources.actors.find(name);
			if (it != m_resources.actors.end()) {
				it->second->AddEntity(m_resources.cameras[i]);
				//outScene->AddCameraNode(it->second);
			}

		}
	}

	// fenyek helyenek kiszedese a scenegraphbol
	if (aiscene->HasLights()) {
		for (i = 0; i < aiscene->mNumLights; i++) {
			aiLight * curr_light = aiscene->mLights[i];

			std::string name = curr_light->mName.C_Str();

			auto it = m_resources.actors.find(name);
			if (it != m_resources.actors.end()) {
				it->second->AddEntity(m_resources.lights[i]);
				//outScene->AddLightNode(it->second);
			}
		}
	}
}

void Grafkit::AssimpLoader::AssimpLoadAnimations(SceneRef &outScene)
{
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
}

// ================================================================================================================================================================
// 
// ================================================================================================================================================================

/**
* AI forras scenebol felepiti a scenegraphot a betoltott nodeokbol
* @param repo a betoltott elemek mapje
* @param ai_node feldolgozas alatti ai scenegraph node
* @param actor_node az a scenegraph node, mai epp feltoltunk
* @param maxdepth stack overflow ellen
*/
void Grafkit::AssimpLoader::AssimpParseScenegraphNode(aiNode * ai_node, ActorRef & actor_node, int maxdepth)
{
	size_t i = 0, j = 0, k = 0;

	if (!ai_node)
		return;

	if (maxdepth < 0)
		return;

	ActorRef actor = new Actor();

	LOGGER(
		int depth = 1024 - maxdepth; char tab[1024]; tab[depth] = 0;
	while (depth--) tab[depth] = ' ';
	);

	const char * name = ai_node->mName.C_Str();

	LOGGER(Log::Logger().Trace(" %s%s [%d]", tab, name, ai_node->mNumMeshes));

	LOGGER(
		size_t buflen = strlen(tab) + 8 * ai_node->mNumMeshes + 256;
	char kbuf[4096];
	sprintf_s(kbuf, 4096, "%s meshes:", tab);
	);

	actor->SetName(name);

	// ---
	for (i = 0; i < ai_node->mNumMeshes; i++) {
		UINT mesh_id = ai_node->mMeshes[i];
		if (m_resources.models.size() <= mesh_id)
			throw EX(OutOfBoundsException);
		actor->AddEntity(m_resources.models[mesh_id]);
		LOGGER(sprintf_s(kbuf, buflen, "%s %d,", kbuf, i));
	}

	LOGGER(if (ai_node->mNumMeshes) Log::Logger().Trace(" %s", kbuf));

	actor->SetName(ai_node->mName.C_Str());
	actor->Matrix().Set(ai4x4MatrixToFWMatrix(&ai_node->mTransformation));

	// ezekkel mit lehet meg kezdeni:
	//for (i = 0; i < ai_node->mMetaData->mNumProperties; i++) {
	//	//aiMetadata *meta = ai_node->mMetaData->Get( // ->Get(ai_node->mMetaData->mKeys[i]);
	//	// meta->
	//}

	m_resources.actors[actor->GetName()] = actor;

	// next nodes
	for (i = 0; i < ai_node->mNumChildren; i++) {
		ActorRef child = nullptr;
		AssimpParseScenegraphNode(ai_node->mChildren[i], child, maxdepth - 1);
		actor->AddChild(child);
	}

	actor_node = actor;
}

// ================================================================================================================================================================
// 
// ================================================================================================================================================================


Grafkit::Matrix Grafkit::AssimpLoader::ai4x4MatrixToFWMatrix(aiMatrix4x4 * m)
{
	if (!m)
		throw EX(NullPointerException);

#if 0
	return Grafkit::Matrix(
		m->a1, m->a2, m->a3, m->a4,
		m->b1, m->b2, m->b3, m->b4,
		m->c1, m->c2, m->c3, m->c4,
		m->d1, m->d2, m->d3, m->d4
	);
#else
	return Grafkit::Matrix(
		m->a1, m->b1, m->c1, m->d1,
		m->a2, m->b2, m->c2, m->d2,
		m->a3, m->b3, m->c3, m->d3,
		m->a4, m->b4, m->c4, m->d4
	);
#endif 
}

/**
* AI materialbol textura infot extractol, majd resouce managerbol kiszedi a texturat hozza.
* @param source ai textura tipus; milyen mapra van felhuzva - e.g. Diffuse, Normal, ...
* @param material ai forras material, amibol a texturat ki kell venni
* @param submode ai materialon belul a teztura indexe
* @param resman globalis resurce manager
* @return textura resource peldany referencia
*/
TextureResRef Grafkit::AssimpLoader::assimpTexture(enum aiTextureType source, aiMaterial* material, int subnode, Grafkit::IResourceManager * const & resman)
{
	aiString path;
	TextureResRef  texture;
	aiReturn result = material->GetTexture(source, subnode, &path);

	std::string name = path.C_Str();

	if (result == AI_SUCCESS /*&& path.data[0]*/) {
		LOGGER(Log::Logger().Trace("--- %s", name.c_str()));
		texture = resman->Get<TextureRes>(name);
		if (texture.Invalid() || texture->Invalid())
			throw EX_DETAILS(NullPointerException, name.c_str());
	}

	return texture;
}

/**
*/
inline aiVector3D Grafkit::AssimpLoader::crossProduct(aiVector3D a, aiVector3D b) {
	return aiVector3D(
		(a.y*b.z - a.z*b.y),
		(a.z*b.x - a.x*b.z),
		(a.x*b.y - a.y*b.x)
	);
}

/*
*/
inline void Grafkit::AssimpLoader::swap_vertices(aiVector3D *vertices, char order[], char polarity[]) {
	aiVector3D v;
	v.x = (*vertices)[order[0]] * polarity[0];
	v.y = (*vertices)[order[1]] * polarity[1];
	v.z = (*vertices)[order[2]] * polarity[2];
	*vertices = v;
}
