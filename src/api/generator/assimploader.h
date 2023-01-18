#pragma once

#include "exceptions.h"

#include "../core/asset.h"
#include "../core/resource.h"
#include "../core/ResourceBuilder.h"

#include "../render/Scene.h"
#include "../render/model.h"
#include "../render/texture.h"

namespace Grafkit {

	class AssimpLoader : public Grafkit::IResourceBuilder
	{
	public:
		AssimpLoader(/*Grafkit::IAssetRef resource*/std::string source_name, Grafkit::SceneRef & scenegraph);
		~AssimpLoader();

		virtual void load(IResourceManager * const & assman) = 0;

	private:
		/*Grafkit::IAssetRef m_resource;
		Grafkit::Scene * const & m_scenegraph;*/
	};

}

// exceptions
DEFINE_EXCEPTION(AssimpParseException, 4000, "Error while parsing file via ASSIMP");
