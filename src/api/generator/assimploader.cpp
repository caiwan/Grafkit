#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/matrix4x4.h"

#include "../math/matrix.h"

#include "assimploader.h"

using namespace FWdebugExceptions;

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

// ================================================================================================================================================================
// Head
// ================================================================================================================================================================

FWmodel::AssimpLoader::AssimpLoader(FWassets::IResourceRef resource, FWassets::IRenderAssetManager * const & assman, FWrender::Scenegraph * const & scenegraph) : 
	IRenderAssetGenerator(assman), 
	m_scenegraph(scenegraph), m_resource(resource)
{

}

FWmodel::AssimpLoader::~AssimpLoader()
{
}

// ================================================================================================================================================================
// It does the trick
// ================================================================================================================================================================
void FWmodel::AssimpLoader::operator()()
{
	Assimp::Importer importer;
	const aiScene *scene = (importer.ReadFileFromMemory(this->m_resource->GetData(), this->m_resource->GetSize(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenNormals));

	if (!scene) {
		throw EX_DETAILS(AssimpParseException, importer.GetErrorString());
	}

	// build texture -- material LUT
	// -- load materials
	// -- -- load textures

	// scene loading
	// -- meshes
	// -- faces
	// -- luukup materials

	// load cameras

	// load lights

	// build scenegraph
}

