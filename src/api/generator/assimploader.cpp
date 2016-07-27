#include "../stdafx.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <stack>
#include <vector>

#include "../utils/resource.h"
#include "../utils/ResourceManager.h"

#include "../render/renderer.h"
#include "../render/texture.h"
#include "../render/Material.h"
#include "../render/model.h"
#include "../render/camera.h"
#include "../render/light.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/matrix4x4.h"

#include "assimploader.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

// ================================================================================================================================================================
// Assimp helpers
// ================================================================================================================================================================

// aiVector3D to float3
#define assimp_v3d_f3(SRC, DST)\
{\
	(DST).x = (SRC).x, (DST).y = (SRC).y, (DST).z = (SRC).z;\
}

#define assimp_v3d_f3_set(SRC, DST)\
(DST((SRC).x, (SRC).y, (SRC).z))

// aiVector3D to float4
#define assimp_v3d_f4(SRC, DST, W)\
{\
	(DST).x = (SRC).x, (DST).y = (SRC).y, (DST).z = (SRC).z, (DST).w = (W);\
}

// aiVector4D to float4
#define assimp_vf4_f4(SRC, DST)\
{\
	(DST).x = (SRC).x, (DST).y = (SRC).y, (DST).z = (SRC).z, (DST).w = (SRC).w;\
}

#define assimp_color_f4(SRC, DST)\
{\
	(DST).x = (SRC).r, (DST).y = (SRC).g, (DST).z = (SRC).b, (DST).z = 1.0f; \
}

// ezeket mashogy nem lehet megoldnai, mert a matkey is egy makro
#define assimpMaterialKey_2_float4(SRC, _AI_ENUM, OUT)\
{\
	aiColor3D ac; float4& fv = (OUT); \
	(SRC)->Get(_AI_ENUM, ac); \
	fv.x = ac.r, fv.y = ac.g, fv.z = ac.b, fv.w = 1.0; \
}

#define assimpMaterialKey_2_float(SRC, _AI_ENUM, OUT) \
{\
	float &scalar = (OUT);\
	(SRC)->Get(_AI_ENUM, scalar);\
}

// blender workaround miatt kell
// vertex order nem oke, (x y z) -> (x -z y)
#if 0
#define assimp_vertices(SRC, DST, W)\
{\
	(DST).x = (SRC).x, (DST).y = -(SRC).z, (DST).z = (SRC).y, (DST).w = (W);\
}
#else 
#define assimp_vertices(SRC, DST, W) assimp_v3d_f4(SRC, DST, W)
#endif

Grafkit::Matrix ai4x4MatrixToFWMatrix(aiMatrix4x4 * m)
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

#endif 
}

namespace {

	///Based on 3ds format's 
	enum texture_type_e {
		// regular types	
		TT_diffuse,	///< 1st map

		TT_alpha,		///< alpha map
		TT_normal,		///< bump map
		TT_shiniess,	///< shininess map
		TT_specular,	///< specular map
		TT_selfillum,	///< self illumination map
		TT_reflect,		///< reflection map
		TT_bump,		///< bump map

		TT_aux, ///< aux texture, used for pretty much everything else

		TT_COUNT	// count
	};

	// AI texture -- internal texture type mapping 
	const struct {
		enum aiTextureType ai;
		enum texture_type_e tt;
	}
	texture_load_map[] = {
		aiTextureType_NONE,		    TT_aux,
		aiTextureType_DIFFUSE,      TT_diffuse,
		aiTextureType_SPECULAR,     TT_specular,
		aiTextureType_AMBIENT,      TT_aux,
		aiTextureType_EMISSIVE,     TT_aux,
		aiTextureType_HEIGHT,       TT_aux,
		aiTextureType_NORMALS,      TT_normal,
		aiTextureType_SHININESS,    TT_shiniess,
		aiTextureType_OPACITY,      TT_alpha,
		aiTextureType_DISPLACEMENT, TT_bump,
		aiTextureType_LIGHTMAP,		TT_aux,
		aiTextureType_REFLECTION,	TT_reflect,
		aiTextureType_UNKNOWN,		TT_aux,
	};


	const char *texture_map_names[TT_COUNT] =
	{
		"t_diffuse",
		"t_alpha",
		"t_normal",
		"t_shiniess",
		"t_specular",
		"t_selfillum",
		"t_reflect",
		"t_bump",
		"t_aux",
	};
}

/**
 * AI materialbol textura infot extractol, majd resouce managerbol kiszedi a texturat hozza. 
 * @param source ai textura tipus; milyen mapra van felhuzva - e.g. Diffuse, Normal, ... 
 * @param material ai forras material, amibol a texturat ki kell venni
 * @param submode ai materialon belul a teztura indexe
 * @param resman globalis resurce manager
 * @return textura resource peldany referencia
*/
TextureResRef assimpTexture(enum aiTextureType source, aiMaterial* material, int subnode, Grafkit::IResourceManager * const & resman)
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

// ================================================================================================================================================================
// Parse assimp scenegraph 
// ================================================================================================================================================================

/* Ezeket toltuk majd be, illetve adjuk a scenegraphoz */

typedef std::map<std::string, ActorRef> actorMap_t;
typedef actorMap_t::iterator actorMap_it;

typedef struct {
	std::vector<MaterialRef> materials;
	std::vector<ModelRef> models;
	std::vector<CameraRef> cameras;
	std::vector<LightRef> lights;
	actorMap_t actors;
} resourceRepo_t;

/**
 * AI forras scenebol felepiti a scenegraphot a betoltott nodeokbol
 * @param repo a betoltott elemek mapje
 * @param ai_node feldolgozas alatti ai scenegraph node
 * @param actor_node az a scenegraph node, mai epp feltoltunk
 * @param maxdepth stack overflow ellen
*/
void assimp_parseScenegraph(resourceRepo_t &repo,  aiNode* ai_node, ActorRef &actor_node, int maxdepth = 1024)
{
	size_t i=0, j=0, k = 0;

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
		if (repo.models.size() <= mesh_id)
			throw EX(OutOfBoundsException);
		actor->AddEntity(repo.models[mesh_id]);
		LOGGER(sprintf_s(kbuf, buflen, "%s %d,", kbuf, i));
	}

	LOGGER(if(ai_node->mNumMeshes) Log::Logger().Trace(" %s", kbuf));

	actor->SetName(ai_node->mName.C_Str());
	actor->Matrix().Set(ai4x4MatrixToFWMatrix(&ai_node->mTransformation));
	
	// ezekkel mit lehet meg kezdeni:
	//for (i = 0; i < ai_node->mMetaData->mNumProperties; i++) {
	//	//aiMetadata *meta = ai_node->mMetaData->Get( // ->Get(ai_node->mMetaData->mKeys[i]);
	//	// meta->
	//}

	repo.actors[actor->GetName()] = actor;

	// next nodes
	for (i = 0; i < ai_node->mNumChildren; i++) {
		ActorRef child = nullptr;
		assimp_parseScenegraph(repo, ai_node->mChildren[i], child, maxdepth - 1);
		actor->AddChild(child);
	}

	actor_node = actor;
}

// ================================================================================================================================================================
// Head
// ================================================================================================================================================================
Grafkit::AssimpLoader::AssimpLoader(std::string source_name, ShaderResRef schemanticSource) : IResourceBuilder(source_name, source_name),
m_schSrc(schemanticSource)
{
}

Grafkit::AssimpLoader::~AssimpLoader()
{
}

IResource * Grafkit::AssimpLoader::NewResource()
{
	return new SceneRes();
}


// ================================================================================================================================================================
// It does the trick
// ================================================================================================================================================================
void Grafkit::AssimpLoader::Load(IResourceManager * const & resman, IResource * source)
{
	SceneResRef dstScene = dynamic_cast<SceneRes*>(source);
	if (dstScene.Invalid()) {
		/// @todo thorw exception, de szerintem nem kell 
		return;
	}

	SceneRef outScene = new Scene();
	// outScene->SetName(m_srcName);

	

	IAssetRef srcAsset = this->GetSourceAsset(resman);

	if (!srcAsset || !srcAsset->GetData())
		throw EX_DETAILS(AssimpParseException, "Nem tudom betolteni a forras assetet");

	Assimp::Importer importer;
	/// @todo genNormals szar. Miert?
	const aiScene *scene = importer.ReadFileFromMemory(srcAsset->GetData(), srcAsset->GetSize(),
		aiProcessPreset_TargetRealtime_Quality | 
		aiProcess_ConvertToLeftHanded |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		//aiProcess_GenNormals | // ez valamiert elszarik
		0
	);

	if (!scene)
		throw EX_DETAILS(AssimpParseException, importer.GetErrorString());

	size_t i = 0, j = 0, k = 0, l = 0;
	resourceRepo_t resourceRepo;
	std::vector<MaterialRef> &materials = resourceRepo.materials;
	std::vector<ModelRef> &models = resourceRepo.models;
	std::vector<CameraRef> &cameras = resourceRepo.cameras;
	std::vector<LightRef> &lights = resourceRepo.lights;

	// -- load materials

	if (scene->HasMaterials()) {
		LOGGER(Log::Logger().Trace("Materials"));
		for (i = 0; i<scene->mNumMaterials; i++) {
			aiString path, name;

			/// @todo milyen material kell 
			MaterialRef material = new BaseMaterial();
			aiMaterial *curr_mat = scene->mMaterials[i];

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
					LOGGER(Log::Logger().Trace("-- texture #%s #%d", texture_map_names[texture_load_map[k].tt], j));
					material->AddTexture(
						assimpTexture(texture_load_map[k].ai, curr_mat, j, resman), texture_map_names[texture_load_map[k].tt]
					);
				}
			}

			assimpMaterialKey_2_float4(curr_mat, AI_MATKEY_COLOR_DIFFUSE, material->GetDiffuse());
			assimpMaterialKey_2_float4(curr_mat, AI_MATKEY_COLOR_AMBIENT, material->GetAmbient());
			assimpMaterialKey_2_float4(curr_mat, AI_MATKEY_COLOR_SPECULAR, material->GetSpecular());
			
			assimpMaterialKey_2_float(curr_mat, AI_MATKEY_SHININESS, material->GetShininess());
			assimpMaterialKey_2_float(curr_mat, AI_MATKEY_SHININESS_STRENGTH, material->GetSpecularLevel());

			materials.push_back(material);
			///@todo itt a resource managert kellene hasznalni
		}
	}
	
	// ----------------------------------------
	// scene loading
	if (scene->HasMeshes()) 
	{
		LOGGER(Log::Logger().Trace("Meshes"));
		// forras sema
		ShaderRef inputSchema = m_schSrc->Get();
		if (inputSchema.Invalid())
			throw EX_DETAILS(NullPointerException, "Nincs meg a shader input sema. Rendezd ugy a loader stacket, hogy a shader elobb legyen, mint a model betolto.");

		for (i = 0; i<scene->mNumMeshes; i++) 
		{
			// -- meshes
			ModelRef mesh = new Model();
			aiMesh* curr_mesh = scene->mMeshes[i];
			
			const char* mesh_name = curr_mesh->mName.C_Str(); //for dbg purposes
			LOGGER(Log::Logger().Trace("- #%d %s", i, mesh_name));
			mesh->SetName(mesh_name);

			SimpleMeshGenerator generator(resman->GetDeviceContext(), inputSchema);
			
			std::vector<float4> vertices;
			std::vector<float2> texuvs;
			std::vector<float4> normals;
			std::vector<float4> tangents;

			for (k = 0; k < curr_mesh->mNumVertices; k++) {
				float4 v; 
				assimp_vertices(curr_mesh->mVertices[k], v, 1.); vertices.push_back(v);
				assimp_vertices(curr_mesh->mNormals[k], v, 1.); normals.push_back(v);
				
				if (curr_mesh->mTextureCoords && curr_mesh->mTextureCoords[0]) {
					// @todo valamiert nincs mindig tangent
					if (curr_mesh->mTangents) {
						assimp_vertices(curr_mesh->mTangents[k], v, 1.); tangents.push_back(v);
					}
					assimp_vertices(curr_mesh->mTextureCoords[0][k], v, 1.); texuvs.push_back(float2(v.x, v.y));
				}
					
			}

			/**/
			generator["POSITION"] = &vertices[0];
			generator["NORMAL"] = &normals[0];

			if (curr_mesh->mTextureCoords && curr_mesh->mTextureCoords[0]) {
				generator["TEXCOORD"] = &texuvs[0];
				if (curr_mesh->mTangents)
					generator["TANGENT"] = &tangents[0];
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

				generator(curr_mesh->mNumVertices, indices.size(), &indices[0], mesh);
			}
			else {
				///@todo arra az esetre is kell valamit kitalalni, amikor nincsenek facek, csak vertexek.
			}

			// -- lookup materials
			int mat_index = curr_mesh->mMaterialIndex;
			if (materials.size()>mat_index)
			{
				mesh->SetMaterial(materials[mat_index]);
			}

			models.push_back(mesh);
			///@todo itt a resource managert kellene hasznalni
		}
	}

	// load cameras
	if (scene->HasCameras()) {
		LOGGER(Log::Logger().Trace("Cameras"));
		for (i = 0; i < scene->mNumCameras; i++) {
			CameraRef camera = new Camera();
			aiCamera *curr_camera = scene->mCameras[i];

			const char* camera_name = curr_camera->mName.C_Str();
			LOGGER(Log::Logger().Trace("- #%d %s", i, camera_name));

			camera->SetName(camera_name);

			camera->SetFOV(180.*curr_camera->mHorizontalFOV / M_PI);
			camera->SetClippingPlanes(curr_camera->mClipPlaneNear, curr_camera->mClipPlaneFar);

			// camera <- assmimp camera
			
			assimp_v3d_f3_set(curr_camera->mPosition, camera->SetPosition);
			assimp_v3d_f3_set(curr_camera->mLookAt, camera->SetLook);
			assimp_v3d_f3_set(curr_camera->mUp, camera->SetUp);

			// itt van meg aspekt, amivel kezdeni lehetne valamit

			cameras.push_back(camera);
		}
	}
#if 1
	// load lights
	if (scene->HasLights()) {
		LOGGER(Log::Logger().Trace("Lights"));
		for (i = 0; i < scene->mNumLights; i++) {
			LightRef light; 
			aiLight *curr_light = scene->mLights[i];

			const char *light_name = curr_light->mName.C_Str();
			LOGGER(Log::Logger().Trace("- #%d %s %d", i, light_name, curr_light->mType));
		

			// light <- assimp light
			switch (curr_light->mType) {
			case aiLightSource_DIRECTIONAL:
				light = new DirectionalLight();
				assimp_v3d_f4(curr_light->mDirection, light->GetDirection(), 0, 0f);
				break;

			case aiLightSource_POINT:
				light = new PointLight();
				assimp_v3d_f4(curr_light->mPosition, light->GetPosition(), 0.0f);
				break;

			case aiLightSource_SPOT:
				light = new SpotLight();
				assimp_v3d_f4(curr_light->mPosition, light->GetPosition(), 1.0f);
				assimp_v3d_f4(curr_light->mDirection, light->GetDirection(), 0.0f);
				(light->GetAngle()) = curr_light->mAngleInnerCone;
				(light->GetFalloff()) = curr_light->mAngleOuterCone - curr_light->mAngleInnerCone;
				break;

			case aiLightSource_AMBIENT:
				light = new AmbientLight();
				break;
			}

			(light->GetConstantAttenuation()) = curr_light->mAttenuationConstant;
			(light->GetLinearAttenuation()) = curr_light->mAttenuationLinear;
			(light->GetQuardicAttenuation()) = curr_light->mAttenuationQuadratic;

			assimp_color_f4(curr_light->mColorAmbient, light->GetAmbient());
			assimp_color_f4(curr_light->mColorDiffuse, light->GetDiffuse());
			assimp_color_f4(curr_light->mColorSpecular, light->GetSpecular());

			///@todo ez kell-e majd?
			light->SetName(light_name);

			lights.push_back(light);
		}
	}

#endif
	// build up scenegraph

	ActorRef root_node = new Actor;
	LOGGER(Log::Logger().Trace("Building scenegraph"));
	assimp_parseScenegraph(resourceRepo, scene->mRootNode, root_node);
	outScene->SetRootNode(root_node);

	/* Workaround: 
	 * a root node matrixat identbe kell tenni, mert vices dolgokat csinal
	 */

	// root_node->Matrix().Identity();

	// --- kamera helyenek kiszedese a scenegraphbol
	if (scene->HasCameras()) {
		for (i = 0; i < scene->mNumCameras; i++) {
			aiCamera *curr_camera = scene->mCameras[i];
			std::string name = curr_camera->mName.C_Str();

			actorMap_it it = resourceRepo.actors.find(name);
			if (it != resourceRepo.actors.end()) {
				it->second->AddEntity(cameras[i]);
				outScene->AddCameraNode(it->second);
			}

		}
	}
	
	// fenyek helyenek kiszedese a scenegraphbol
	if (scene->HasLights()) {
		for (i = 0; i < scene->mNumLights; i++) {
			aiLight * curr_light = scene->mLights[i];

			std::string name = curr_light->mName.C_Str();

			actorMap_it it = resourceRepo.actors.find(name);
			if (it != resourceRepo.actors.end()) {
				it->second->AddEntity(lights[i]);
				outScene->AddLightNode(it->second);
			}
		}
	}

	// --- Animacio kiszedese
	if (scene->HasAnimations()) {
		LOGGER(Log::Logger().Trace("Animation"));
		for (i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation *curr_anim = scene->mAnimations[i];
			aiString name = curr_anim->mName;

			LOGGER(Log::Logger().Trace("- #%d : %s", i, name.C_Str()));

			// itt kell tudni, hogy kihez tartozik majd az animacio

			/* (...) */
		}
	}

	// 3.
	if (dstScene->Valid()) {
		dstScene->Release();
	}
	dstScene->AssingnRef(outScene);
}
