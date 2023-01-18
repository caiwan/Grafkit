#include "ShaderLoader.h"

#include "../utils/logger.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

namespace {
	const char *default_entry_point_names[ShaderType_e::ST_COUNT] = {
		"main",
		"mainVertex",
		"mainPixel",
		"mainCompute",
		"mainGeometry"
	};

	const char *type_names[ShaderType_e::ST_COUNT] = {
		"",
		"vs",
		"ps",
		"cs",
		"gs"
	};
}

Grafkit::ShaderLoader::ShaderLoader(std::string name, std::string sourcename, std::string entrypoint, ShaderType_e type) : Grafkit::IResourceBuilder(name, sourcename),
	m_type(type)
{
	if (entrypoint.empty())
		m_entrypoint = default_entry_point_names[type];
	else
		m_entrypoint = entrypoint;
}

Grafkit::ShaderLoader::~ShaderLoader()
{
}

void Grafkit::ShaderLoader::Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source)
{

	ShaderResourceRef dstSahder = dynamic_cast<ShaderResource*>(source);
	if (dstSahder.Invalid()) {
		return;
	}
	////try 
	{
		Grafkit::IAssetRef asset = this->GetSourceAsset(resman);
		ShaderRef shader = new Shader();
		// load from asset
		if (asset.Valid()) {
			LOGGER(LOG(TRACE) << "Lading shader from resource" << m_type << m_name << "@" << m_entrypoint;);

			ShaderRef shader = new Shader();
			/* @todo getDC (!!!) */
			shader->LoadFromMemory(resman->GetDeviceContext(), m_entrypoint.c_str(), (LPCSTR)asset->GetData(), asset->GetSize(), m_type);
		}
		else {
			///@todo load from compiled shader
			throw EX_DETAILS(NotImplementedMethodException, "Egyelore nem tamogatott a forrasbol valo shader betoltes");
		}

	}

	

	if (dstSahder->Valid()) {
		dstSahder->Release();
	}

	//m_dstResource = outShader;

	// 3.
	// dstSahder->AssingnRef(thing);

}
