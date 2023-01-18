#pragma once 

#include "../render/shader.h"

#include "asset.h"
#include "resource.h"
#include "ResourceManager.h"
#include "ResourceBuilder.h"

namespace Grafkit {

	class ShaderLoader : public FWutils::IResourceBuilder
	{
	public:
		ShaderLoader(std::string name, std::string sourcename, std::string entrypoint, ShaderType_e type);
		~ShaderLoader();

		virtual void Load(FWutils::IResourceManager * const & resman, FWutils::IResource * source);

	protected:
		ShaderType_e m_type;
		std::string m_entrypoint;
	};

}