#pragma once 

#include "../render/shader.h"

#include "../utils/resource.h"
#include "../utils/ResourceManager.h"
#include "../utils/ResourceBuilder.h"

namespace Grafkit {

	class ShaderLoader : public Grafkit::IResourceBuilder{
	public:
		ShaderLoader(std::string name, std::string sourcename, std::string entrypoint);
		~ShaderLoader();

	    void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) override;

	    IResource* NewResource() override;

	protected:
		virtual Shader* NewShader() = 0;
		virtual std::string DefaultEntryPointName() = 0;
		std::string m_entrypoint;
	};


	class VertexShaderLoader : public ShaderLoader {
	public:
		VertexShaderLoader(std::string name, std::string sourcename, std::string entrypoint = "")
			: ShaderLoader(name, sourcename, entrypoint){}
		~VertexShaderLoader(){}

	protected:
	    Shader* NewShader() override { return new VertexShader(); }
	    std::string DefaultEntryPointName() override { return "mainVertex"; }
	};


	class PixelShaderLoader : public ShaderLoader {
	public:
		PixelShaderLoader(std::string name, std::string sourcename, std::string entrypoint = "")
			: ShaderLoader(name, sourcename, entrypoint) {}
		~PixelShaderLoader(){}
	protected:
	    Shader* NewShader() override { return new PixelShader(); }
	    std::string DefaultEntryPointName() override { return "mainPixel"; }
	};

	class GeometryShaderLoader : public ShaderLoader {
	public:
		GeometryShaderLoader(std::string name, std::string sourcename, std::string entrypoint = "")
			: ShaderLoader(name, sourcename, entrypoint) {}
		~GeometryShaderLoader() {}

	protected:
	    Shader* NewShader() override { return new GeometryShader(); }
	    std::string DefaultEntryPointName() override { return "mainGeometry"; }
	};

}