#pragma once 

#include "../render/shader.h"

#include "../utils/asset.h"
#include "../utils/resource.h"
#include "../utils/ResourceManager.h"
#include "../utils/ResourceBuilder.h"

namespace Grafkit {

	class ShaderLoader : public Grafkit::IResourceBuilder
	{
	public:
		ShaderLoader(std::string name, std::string sourcename, std::string entrypoint, ShaderType_e type);
		~ShaderLoader();

		virtual void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source);

		virtual IResource* NewResource();

	protected:
		ShaderType_e m_type;
		std::string m_entrypoint;
	};

}