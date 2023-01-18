#pragma once 

#include "common.h"

#include "resource/ResourceBuilder.h"

namespace Grafkit {

	class ShaderLoader : public IResourceBuilder{
	public:
		ShaderLoader(std::string name, std::string sourcename, std::string entrypoint, std::string uuid="");
		~ShaderLoader();

	    void Load(IResourceManager * const & resman, IResource * source) override;

	    IResource* NewResource() override;

	protected:
		virtual Shader* NewShader() = 0;
		virtual std::string DefaultEntryPointName() = 0;
		std::string m_entrypoint;
	};


	class VertexShaderLoader : public ShaderLoader {
	public:
		VertexShaderLoader(std::string name, std::string sourcename, std::string entrypoint = "", std::string uuid = "")
			: ShaderLoader(name, sourcename, entrypoint, uuid){}
		~VertexShaderLoader(){}

	protected:
	    Shader* NewShader() override;
	    std::string DefaultEntryPointName() override { return "mainVertex"; }
	};


	class PixelShaderLoader : public ShaderLoader {
	public:
		PixelShaderLoader(std::string name, std::string sourcename, std::string entrypoint = "", std::string uuid = "")
			: ShaderLoader(name, sourcename, entrypoint, uuid) {}
		~PixelShaderLoader(){}
	protected:
	    Shader* NewShader() override;
	    std::string DefaultEntryPointName() override { return "mainPixel"; }
	};

	class GeometryShaderLoader : public ShaderLoader {
	public:
		GeometryShaderLoader(std::string name, std::string sourcename, std::string entrypoint = "", std::string uuid = "")
			: ShaderLoader(name, sourcename, entrypoint, uuid) {}
		~GeometryShaderLoader() {}

	protected:
	    Shader* NewShader() override;
	    std::string DefaultEntryPointName() override { return "mainGeometry"; }
	};

}