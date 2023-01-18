#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/matrix4x4.h"

#include "assimploader.h"

using namespace FWdebugExceptions;

// ================================================================================================================================================================
// Head
// ================================================================================================================================================================

FWmodel::AssimpLoader::AssimpLoader(FWassets::IRenderAssetManager * const & assman, FWrender::Scenegraph * const & scenegraph, const char * filename):
	IRenderAssetGenerator(assman),
	m_scenegraph(scenegraph)
{
	m_mem.is_file = 0;
	m_mem.data = nullptr;
	m_mem.length = 0;

	// todo ... 
	throw EX(NotImplementedMethodException);
}

FWmodel::AssimpLoader::AssimpLoader(FWassets::IRenderAssetManager * const & assman, FWrender::Scenegraph * const & scenegraph, const WCHAR * filename) :
	IRenderAssetGenerator(assman),
	m_scenegraph(scenegraph)
{
	m_mem.is_file = 0;
	m_mem.data = nullptr;
	m_mem.length = 0;

	//todo ...
	throw EX(NotImplementedMethodException);
}

FWmodel::AssimpLoader::AssimpLoader(FWassets::IRenderAssetManager * const & assman, FWrender::Scenegraph * const & scenegraph, const void * data, size_t length) :
	IRenderAssetGenerator(assman), 
	m_scenegraph(scenegraph)
{
	m_mem.is_file = 0;
	m_mem.data = const_cast<void*>(data);
	m_mem.length = length;
}

FWmodel::AssimpLoader::~AssimpLoader()
{
	if (this->m_mem.is_file) {
		free(this->m_mem.data);
	}
}

// ================================================================================================================================================================
// It does the trick
// ================================================================================================================================================================
void FWmodel::AssimpLoader::operator()()
{

}

