#pragma once

#include "utils/exceptions.h"

#include "utils/asset.h"
#include "utils/resource.h"
#include "utils/ResourceBuilder.h"
			
#include "render/Scene.h"
#include "render/model.h"
#include "render/texture.h"

namespace Grafkit {

	class AssimpLoader : public Grafkit::IResourceBuilder
	{
	public:
		/**
		@param source_name a forras file neve
		@param schemanticSource a forras vertex shader, amibol az input layout jon
		*/
		AssimpLoader(std::string source_name, ShaderResRef schemanticSource);
		~AssimpLoader();

		virtual void Load(IResourceManager * const & resman, IResource * source);

		virtual IResource* NewResource();

	private:
		ShaderResRef m_schSrc;
	};

}

// exceptions
DEFINE_EXCEPTION(AssimpParseException, 4000, "Error while parsing file via ASSIMP");
