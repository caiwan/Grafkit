#define _USE_MATH_DEFINES
#include <cmath>
#include <stack>
#include <vector>

#include "../core/renderassets.h"

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

#include "../math/matrix.h"

#include "assimploader.h"

using namespace FWrender;
using namespace FWdebugExceptions;
using FWassets::IRenderAsset;
using FWassets::IRenderAssetRepository;
using FWassets::IRenderAssetManager;

// ================================================================================================================================================================
// Assimp helpers
// ================================================================================================================================================================

// aiVector3D to float3
#define assimp_v3d_f3(SRC, DST)\
{\
	(DST).x = (SRC).x, (DST).y = (SRC).y, (DST).z = (SRC).z;\
}

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

FWmath::Matrix ai4x4MatrixToFWMatrix(aiMatrix4x4 * m)
{
	if (!m) 
		throw EX(NullPointerException);

	return FWmath::Matrix(
		m->a1, m->a2, m->a3, m->a4,
		m->b1, m->b2, m->b3, m->b4,
		m->c1, m->c2, m->c3, m->c4,
		m->d1, m->d2, m->d3, m->d4
		);
}

namespace {
	// AI texture -- internal texture type mapping 
	const struct {
		enum aiTextureType ai;
		enum texture_type_e tt;
	}
	texture_load_map[] = {
		aiTextureType_NONE,		TT_aux,
		aiTextureType_DIFFUSE,  TT_diffuse,
		aiTextureType_SPECULAR, TT_specular,
		aiTextureType_AMBIENT,  TT_aux,
		aiTextureType_EMISSIVE, TT_aux,
		aiTextureType_HEIGHT,   TT_aux,
		aiTextureType_NORMALS,   TT_normal,
		aiTextureType_SHININESS, TT_shiniess,
		aiTextureType_OPACITY,   TT_alpha,
		aiTextureType_DISPLACEMENT, TT_bump,
		aiTextureType_LIGHTMAP,		TT_aux,
		aiTextureType_REFLECTION,	TT_reflect,
		aiTextureType_UNKNOWN,		TT_aux,
	};
}

TextureAssetRef assimpTexture(enum aiTextureType source, aiMaterial* material, int subnode, FWassets::IRenderAssetManager * const & assman)
{
	aiString path;
	TextureAssetRef textureAsset;
	aiReturn result = material->GetTexture(source, subnode, &path);

	std::string name = path.C_Str();

	if (result == AI_SUCCESS && path.data[0]) {
		textureAsset = (TextureAsset*)assman->GetRepository(ROOT_REPOSITORY)->GetObjectByName(TEXTURE_BUCKET, name).Get();
	}

	return textureAsset;
}

// ================================================================================================================================================================
// Parse assimp scenegraph 
// ================================================================================================================================================================

///@todo assmant hasznlaj
void assimp_parseScenegraph(IRenderAssetRepository *& repo,  aiNode* ai_node, Actor** actor_node, int maxdepth = TREE_MAXDEPTH)
{
	size_t i=0, j=0, k = 0;

	if (!ai_node)
		return;
	
	if (maxdepth < 0)
		return;

	Actor* actor = new Actor();
	
	for (i = 0; i < ai_node->mNumMeshes; i++) {
		UINT mesh_id = ai_node->mMeshes[i];
		actor->GetEntities().push_back((Model*)repo->GetObjPtr(MODEL_BUCKET, mesh_id).Get());
	}

	actor->SetName(ai_node->mName.C_Str());
	actor->Matrix() = ai4x4MatrixToFWMatrix(&ai_node->mTransformation);

	// next nodes
	for (i = 0; i < ai_node->mNumChildren; i++) {
		Actor *child = nullptr;
		assimp_parseScenegraph(repo, ai_node->mChildren[i], &child, maxdepth - 1);
		actor->AddChild(child);
	}

	*actor_node = actor;
}

// ================================================================================================================================================================
// Head
// ================================================================================================================================================================

namespace {
	static size_t count;

	///@todo ez a nevezesi modszertan nem jo; valami mas megoldast kell keresni majd
	std::string GetCounter() {
		char buffer[16];
		sprintf_s(buffer, "assimp:%d:", count);
		count++;
		return buffer;
	}
}

FWmodel::AssimpLoader::AssimpLoader(FWassets::IResourceRef resource, FWrender::Scene * const & scenegraph) :
	IRenderAssetBuilder(),
	m_scenegraph(scenegraph), m_resource(resource)
	// m_name_prefix()
{
	// m_name_prefix = GetCounter();
}

FWmodel::AssimpLoader::~AssimpLoader()
{
}

// ================================================================================================================================================================
// It does the trick
// ================================================================================================================================================================
void FWmodel::AssimpLoader::operator()(FWassets::IRenderAssetManager * const &assman)
{
	Assimp::Importer importer;
	/// @todo genNormals szar. Miert?
	const aiScene *scene = importer.ReadFileFromMemory(this->m_resource->GetData(), this->m_resource->GetSize(), 
		aiProcessPreset_TargetRealtime_Quality | /*aiProcess_GenNormals |*/ 0
	);

	if (!scene) {
		throw EX_DETAILS(AssimpParseException, importer.GetErrorString());
	}

	IRenderAssetRepository* asset_repo = assman->GetRepository(GetCounter());

	int i = 0, j = 0, k = 0, l = 0;
	std::vector<MaterialRef> materials;
	std::vector<ModelRef> models;
	std::vector<CameraRef> cameras;
	std::vector<LightRef> lights;

	// -- load materials
	if (scene->HasMaterials()) {
		for (i = 0; i<scene->mNumMaterials; i++) {
			aiString path, name;

			/// @todo milyen material kell 
			MaterialRef material = new MaterialBase();
			aiMaterial *curr_mat = scene->mMaterials[i];

			if (curr_mat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) 
				material->SetName(name.C_Str());

			j = 0;
			aiReturn texFound = AI_FAILURE;

			// -- -- load textures
			// textura-> material 
			for (k = 0; k < sizeof(texture_load_map) / sizeof(texture_load_map[0]); k++) {
				for (j = 0; j < curr_mat->GetTextureCount(texture_load_map[k].ai); j++) {
					material->AddTexture(assimpTexture(texture_load_map[k].ai, curr_mat, j, assman), texture_load_map[k].tt);
				}
			}

			assimpMaterialKey_2_float4(curr_mat, AI_MATKEY_COLOR_DIFFUSE, material->GetDiffuse());
			assimpMaterialKey_2_float4(curr_mat, AI_MATKEY_COLOR_AMBIENT, material->GetAmbient());
			assimpMaterialKey_2_float4(curr_mat, AI_MATKEY_COLOR_SPECULAR, material->GetSpecular());
			
			assimpMaterialKey_2_float(curr_mat, AI_MATKEY_SHININESS, material->GetShininess());
			assimpMaterialKey_2_float(curr_mat, AI_MATKEY_SHININESS_STRENGTH, material->GetSpecularLevel());

			// -> valahol a loaderen kivul kell megtenni a shader kijelolest, illetve betoltest
			///@todo a shadereket lehessen filterezni, vagy valamilyen modon customizalni, ha lehetne vegre~
			ShaderAssetRef shader_fs = (ShaderAsset*)assman->GetRepository(ROOT_REPOSITORY)->GetObjectByName(SHADER_BUCKET, "default.hlsl:vertex").Get();
			material->SetShader(shader_fs);

			materials.push_back(material);
			asset_repo->AddObject(material);
		}
	}
	
	// Itt letre kell hozni egy uj shadert -> igazabol ezt valahol kivul kellene megtenni
	
	///@todo a shadereket lehessen filterezni, vagy valamilyen modon customizalni, ha lehetne vegre
	ShaderAssetRef shader_vs = (ShaderAsset*)assman->GetRepository(ROOT_REPOSITORY)->GetObjectByName(SHADER_BUCKET, "default.hlsl:vertex").Get();
	
	/*
	if (shader_vs.Invalid())
		throw EX_DETAILS(AssimpParseException, "Could not found default vertex shader"); 
	*/

	// ----------------------------------------
	// scene loading
	if (scene->HasMeshes()) 
	{
		for (i = 0; i<scene->mNumMeshes; i++) 
		{
			// -- meshes
			
			ModelRef model = new Model();
			aiMesh* curr_mesh = scene->mMeshes[i];

			aiString name;
			const char* mesh_name = curr_mesh->mName.C_Str(); //for dbg purposes
			model->SetName(mesh_name);	///@todo valami modelszamlalora utalo nyom/jel is legyen benne 

			///@todo ezeken a neveket ki kell pakolni valahova
			///@todo kell egy olyan mesh generator, ami nem a shaderbol szedi ossze az input layoutot
			SimpleMeshGenerator generator(assman->GetDeviceContext(), shader_vs);
			generator["POSITION"] = curr_mesh->mVertices; 
			generator["TEXCOORD"] = curr_mesh->mTextureCoords[0];  ///@todo ha tobb textura van akkor toltse be azokat is majd 
			generator["NORMAL"] = curr_mesh->mNormals;
			generator["TANGENT"] = curr_mesh->mTangents; 

			// -> a generalas reszet is a pointerek es a shader beallitasa utan 
			///@todo a shadert lehessen a generalas elott geallitani, ne pedig a generator konstruktoraban, sot; ezzel lehetove kellene tenni, hogy csaerelheto legyen a shader a scenegraph eloallitasa utan, de meg a redner elott a preloader szekvenciaban~

			// -- faces

			///@todo az indexek gyujteset lehessen kulturaltabb modon is vegezni valahogy
			std::vector<int> indices;

			for (j = 0; j < curr_mesh->mNumFaces; j++)
			{
				aiFace *curr_face = &curr_mesh->mFaces[j];
				for (k = 0; k < curr_face->mNumIndices; k++)
					indices.push_back(curr_face->mIndices[k]);
			}

			generator(curr_mesh->mNumVertices, indices.size(), &indices[0], model);

			// -- lookup materials
			int mat_index = curr_mesh->mMaterialIndex;
			if (materials.size()>mat_index)
			{
				model->SetMaterial(materials[mat_index]);
			}

			models.push_back(model);
			asset_repo->AddObject(model);
		}
	}

	// load cameras
	if (scene->HasCameras()) {
		for (j = 0; j < scene->mNumCameras; j++) {
			CameraRef camera = new Camera();
			aiCamera *curr_camera = scene->mCameras[j];

			camera->SetFOV(180.*curr_camera->mHorizontalFOV / M_PI);
			camera->SetClippingPlanes(curr_camera->mClipPlaneNear, curr_camera->mClipPlaneFar);

			// camera <- assmimp camera
			assimp_v3d_f3(curr_camera->mPosition, camera->GetPosition());
			assimp_v3d_f3(curr_camera->mLookAt, camera->GetLookAt());
			assimp_v3d_f3(curr_camera->mUp, camera->GetUp());

			// itt van meg aspekt, amivel kezdeni lehetne valamit

			// --
			std::string name(curr_camera->mName.C_Str());

			camera->SetName(name);
			asset_repo->AddObject(camera);
		}
	}

	// load lights
	if (scene->HasLights()) {
		for (j = 0; j < scene->mNumLights; j++) {
			LightRef light; 
			aiLight *curr_light = scene->mLights[i];

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


			light->SetName(std::string(curr_light->mName.C_Str()));

			lights.push_back(light);
			asset_repo->AddObject(light);
		}
	}

	// build up scenegraph

# if 0 // ez pedig tok meno lett volna, ha mukodik rekurzio nelkuk,majd legkozelebb

	struct stack_elem_t {
		aiNode* ai_node;
		Actor* actor_node;
		size_t j;
	};

	std::stack<struct stack_elem_t> node_stack;
	
	stack_elem_t s;
	s.actor_node = new Actor;
	s.ai_node = scene->mRootNode;
	s.j = 0;

	//aiNode* curr_node = scene->mRootNode;

	while (! node_stack.empty() || s.ai_node) {
		if (s.ai_node) {
			// setup node
			// s.actor_node = new Actor();

			// visit next node 
			if (s.ai_node->mNumChildren > 0) {
				// push
				s.j = 0;
				s.actor_node = nullptr;
				node_stack.push(s);
			}
			s.ai_node = s.ai_node->mChildren[s.j];
			s.j++;
		}
		else {
			// pop
			s = node_stack.top();
			node_stack.pop();
		}
	}

	m_scenegraph->SetRootNode(s.actor_node);

#else // fallback: recursive fill 
	Actor* root_node = new Actor;
	assimp_parseScenegraph(asset_repo, scene->mRootNode, &root_node);
	m_scenegraph->SetRootNode(root_node);

#endif

	// kamera helyenek kiszedese a scenegraphbol
	if (scene->HasCameras()) {
	}
	
	// fenyek helyenek kiszedese a scenegraphbol
	if (scene->HasLights()) {
	}

	// animation - ha kell a jovoben; 
	if (scene->HasAnimations()) {
	
	}
}