#include "../stdafx.h"

#include "ShaderLoader.h"

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

	ShaderResRef dstSahder = dynamic_cast<ShaderRes*>(source);
	if (dstSahder.Invalid()) {
		return;
	}
	
	Grafkit::IAssetRef asset = this->GetSourceAsset(resman);
	ShaderRef shader = new Shader();
	// load from asset
	if (asset.Valid()) {
		LOGGER(LOG(TRACE) << "Lading shader from resource" << m_type << m_name << "@" << m_entrypoint;);
		shader->LoadFromMemory(resman->GetDeviceContext(), m_entrypoint.c_str(), (LPCSTR)asset->GetData(), asset->GetSize(), m_type);
	}
	else {
		///@todo load from compiled shader
		throw EX_DETAILS(NotImplementedMethodException, "Egyelore nem tamogatott a forrasbol valo shader betoltes");
	}


	// 2.
	if (dstSahder->Valid()) {
		dstSahder->Release();
	}

	// 3.
	dstSahder->AssingnRef(shader);

}

IResource * Grafkit::ShaderLoader::NewResource()
{
	ShaderRes* res = new ShaderRes();
	return res;
}
