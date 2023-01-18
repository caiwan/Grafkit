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
		///@todo a shadernel tobb forrasbol, tobbfele tipusu shader, tobb entrypointbol betolheto. Ezt valahogyan kezelni kellene a jovoben.
		ShaderLoader(std::string name, std::string filename, std::string entrypoint, ShaderType_e type, ShaderResRef & shaderasset);
		~ShaderLoader();

		virtual void load (Grafkit::IResourceManager * const & assman);

	protected:
		ShaderType_e m_type;
		std::string m_entrypoint;
	};

}