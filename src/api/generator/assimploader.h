#pragma once

#include "exceptions.h"

#include "../core/renderassets.h"
#include "../render/Scenegraph.h"
#include "../render/model.h"
#include "../render/texture.h"

namespace FWmodel {

	class AssimpLoader : public FWassets::IRenderAssetBuilder
	{
	public:
		AssimpLoader(FWassets::IResourceRef resource, FWrender::Scene * const & scenegraph);
		~AssimpLoader();

		virtual void operator () (FWassets::IRenderAssetManager * const &assman);

	private:
		FWassets::IResourceRef m_resource;
		FWrender::Scene * const & m_scenegraph;
		std::string m_name_prefix; //name prefix for every created objects
	};

}

// exceptions
DEFINE_EXCEPTION(AssimpParseException, 4000, "Error while parsing file via ASSIMP");
