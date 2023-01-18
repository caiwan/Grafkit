#include "../render/renderer.h"
#include "../render/texture.h"
#include "../render/Material.h"
#include "../render/model.h"

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

// ================================================================================================================================================================
// Assimp helpers
// ================================================================================================================================================================

// ezeket mashogy nem lehet megoldnai, mert a matkey is egy makro
#define assimpMaterialKey_2_float4(SRC, _AI_ENUM, OUT)\
{\
	aiColor3D ac; float4& fv = OUT; \
	SRC->Get(_AI_ENUM, ac); \
	fv.x = ac.r, fv.y = ac.g, fv.z = ac.b, fv.w = 1.0; \
}

#define assimpMaterialKey_2_float(SRC, _AI_ENUM, OUT) \
{\
	float &scalar = OUT;\
	SRC->Get(_AI_ENUM, scalar);\
}

FWmath::Matrix ai4x4MatrixToFWMatrix(void* _m)
{
	if (!_m) throw EX(NullPointerException);
	aiMatrix4x4 *m = (aiMatrix4x4 *)_m;
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

TextureRef assimpTexture(enum aiTextureType source, aiMaterial* material, int subnode, FWassets::IRenderAssetManager * const & assman)
{
	aiString path;
	TextureRef texture;
	aiReturn result = material->GetTexture(source, subnode, &path);

	std::string name = path.C_Str();

	if (result == AI_SUCCESS && path.data[0]) {
		// obj lut goez here?
		texture = (Texture*)(assman->GetObjectByName(IRenderAsset::RA_TYPE_Texture, name));
		
		///@todo :Ez nem kell majd ide, lehessen felirni a generatorokat az asset managerre is, es o allitsa elo a texturat, ha lehet
		if (texture.Invalid()) {
			TextureFromBitmap txgen(assman->GetResourceFactory()->GetResourceByName(name), texture);
			txgen(assman);
			
			texture->SetName(name);
			assman->AddObject(texture.Get());
		}
	}

	return texture;
}

// ================================================================================================================================================================
// Head
// ================================================================================================================================================================

FWmodel::AssimpLoader::AssimpLoader(FWassets::IResourceRef resource, FWrender::Scenegraph * const & scenegraph) : 
	IRenderAssetBuilder(), 
	m_scenegraph(scenegraph), m_resource(resource)
{

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

	int i = 0, j = 0, k = 0, l = 0;

	// build texture -- material LUT
	std::vector<MaterialRef> materials;

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

			materials.push_back(material);
			assman->AddObject(material.Get());
		}
	}
	

	// scene loading
	if (scene->HasMeshes()) 
	{
		for (i = 0; i<scene->mNumMeshes; i++) 
		{
			// -- meshes
			
			Model *model = new Model();
			aiMesh* curr_mesh = scene->mMeshes[i];

			aiString name;
			const char* mesh_name = curr_mesh->mName.C_Str(); //for dbg purposes
			model->SetName(mesh_name);

			//model->pushVert(reinterpret_cast<vec3float*>(curr_mesh->mVertices), curr_mesh->mNumVertices);
			//model->pushTextureUV(reinterpret_cast<vec3float*>(curr_mesh->mTextureCoords[0]), curr_mesh->mNumVertices);	///@todo tobbfele texuv is lehet
			//model->pushNormals(reinterpret_cast<vec3float*>(curr_mesh->mNormals), curr_mesh->mNumVertices);
			//model->pushTangents(reinterpret_cast<vec3float*>(curr_mesh->mTangents), curr_mesh->mNumVertices);	//ha ilyen is van, akkor eljunk vele

			// ujra kell ezt egy picit tervezni megint :C 

			// shader hol van ? 
			//SimpleMeshGenerator generator(render, shader_vs);
			//generator["POSITION"] = //(void*)FWBuiltInData::cubeVertices;
			//generator["TEXCOORD"] = //(void*)FWBuiltInData::cubeTextureUVs;
			//generator["NORMAL"] = //(void*)FWBuiltInData::cubeTextureUVs;
			//generator["TANGENT"] = //(void*)FWBuiltInData::cubeTextureUVs;

			//generator(FWBuiltInData::cubeVertexLength, FWBuiltInData::cubeIndicesLength, FWBuiltInData::cubeIndices, model);


			// -- faces
			j = curr_mesh->mNumFaces;

			while (j--) {
				aiFace *curr_face = &curr_mesh->mFaces[j];
				///*offset = */ model->pushMesh(curr_face->mIndices, curr_face->mNumIndices /*, offset */);
			}

			// -- luukup materials
			//int mat_index = curr_mesh->mMaterialIndex;
			//if (materials.size()>mat_index)
			//{
			//	//Material *mat = materials[mat_index];
			//	model->setMaterial(material_table[mat_index].material);
			//	if (material_table[mat_index].textures) {
			//		k = material_table[mat_index].textures->size();
			//		while (k--) {
			//			model->SetTexture(k, (*material_table[mat_index].textures)[k]);
			//		}
			//		delete material_table[mat_index].textures;
			//		material_table[mat_index].textures = NULL;
			//	}
			//}

			//FWmath::Matrix modelview = model->getModelviewMatrix();
			//this->models.push(model);
			//assman->AddObject(model);
		}
	}

	// load cameras

	// load lights

	// build scenegraph
}

