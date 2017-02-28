#pragma once 

#include "../render/shader.h"

#include "../utils/asset.h"
#include "../utils/resource.h"
#include "../utils/ResourceManager.h"
#include "../utils/ResourceBuilder.h"

namespace Grafkit {

	class ShaderLoader : public Grafkit::IResourceBuilder{
	public:
		ShaderLoader(std::string name, std::string sourcename, std::string entrypoint);
		~ShaderLoader();

		virtual void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source);

		virtual IResource* NewResource();

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
		virtual Shader* NewShader() { return new VertexShader(); }
		virtual std::string DefaultEntryPointName() { return "mainVertex"; }
	};


	class PixelShaderLoader : public ShaderLoader {
	public:
		PixelShaderLoader(std::string name, std::string sourcename, std::string entrypoint = "")
			: ShaderLoader(name, sourcename, entrypoint) {}
		~PixelShaderLoader(){}
	protected:
		virtual Shader* NewShader() { return new PixelShader(); }
		virtual std::string DefaultEntryPointName() { return "mainPixel"; }
	};

	class GeometryShaderLoader : public ShaderLoader {
	public:
		GeometryShaderLoader(std::string name, std::string sourcename, std::string entrypoint = "")
			: ShaderLoader(name, sourcename, entrypoint) {}
		~GeometryShaderLoader() {}

	protected:
		virtual Shader* NewShader() { return new GeometryShader(); }
		virtual std::string DefaultEntryPointName() { return "mainGeometry"; }
	};

}