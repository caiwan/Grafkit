#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/matrix4x4.h"

#include "utils/exceptions.h"

#include "utils/asset.h"
#include "utils/resource.h"
#include "utils/ResourceBuilder.h"
			
#include "render/Scene.h"
#include "render/model.h"
#include "render/texture.h"

namespace Grafkit {

	class AssimpLoader
	{
	public:
		AssimpLoader(void* data, size_t length);
		~AssimpLoader();

		SceneResRef Load();

	private:
		const void* m_data;
		size_t m_length;
	};

}

// exceptions
DEFINE_EXCEPTION(AssimpParseException, 4000, "Error while parsing file via ASSIMP");
