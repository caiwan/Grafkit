#pragma once 

#include "../render/shader.h"

#include "../core/asset.h"
#include "../core/resource.h"
#include "../core/ResourceBuilder.h"
#include "../core/ResourceManager.h"

namespace Grafkit {

	class ShaderLoader : public Grafkit::IResourceBuilder
	{
	public:
		///@todo leforditott shadert is tudjon elotolteni
		ShaderLoader(Grafkit::IAssetRef resource, ShaderType_e type, ShaderResRef shaderasset);
		~ShaderLoader();// {}

		void SetEntryPoint(std::string entrypoint);

		virtual void operator () (Grafkit::IResourceManager * const & assman);

	protected:
		Grafkit::IAssetRef m_resource;
		ShaderResRef m_in;
		ShaderType_e m_type;
		std::string m_entrypoint;
	};

}