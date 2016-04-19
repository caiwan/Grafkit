#pragma once

#include "../utils/exceptions.h"

#include "../utils/asset.h"
#include "../utils/resource.h"
#include "../utils/ResourceBuilder.h"
			   
#include "../render/Scene.h"
#include "../render/model.h"
#include "../render/texture.h"

namespace Grafkit {

	class AssimpLoader : public Grafkit::IResourceBuilder
	{
	public:
		AssimpLoader(std::string source_name);
		~AssimpLoader();

		virtual void Load(IResourceManager * const & resman, IResource * source);

		virtual IResource* NewResource();
	};

}

// exceptions
DEFINE_EXCEPTION(AssimpParseException, 4000, "Error while parsing file via ASSIMP");
