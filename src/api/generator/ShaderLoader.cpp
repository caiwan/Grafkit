#include "ShaderLoader.h"

#include "easyloggingpp.h"

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

Grafkit::ShaderLoader::ShaderLoader(std::string name, std::string sourcename, std::string entrypoint, ShaderType_e type, ShaderResRef & shaderasset) : Grafkit::IResourceBuilder(name, sourcename, shaderasset),
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

///@todo a shader betoltes menjen on-the-fly, es ne repositorybol
void Grafkit::ShaderLoader::load(Grafkit::IResourceManager * const & assman)
{
	ShaderResRef outShader = (ShaderResRef_t)m_dstResource;
	
	if (outShader.Invalid()) {
		outShader = new ShaderRes();
	}

	//try 
	{
		IAssetRef asset = this->GetSourceAsset(assman);
		ShaderRef shader = new Shader();
		// load from asset
		if (asset.Valid()) {
			LOG(TRACE) << "Lading shader from resource" << m_type << m_name << "@" << m_entrypoint; // << m_in->GetUUID();
			shader->LoadFromMemory(assman->GetDeviceContext(), m_entrypoint.c_str(), (LPCSTR)asset->GetData(), asset->GetSize(), m_type);
		}
		else {
			///@todo load from compiled shader
			throw EX_DETAILS(NotImplementedMethodException, "Egyelore nem tamogatott a forrasbol valo shader betoltes");
		}

		// replace shader
		if (outShader->Valid())
			outShader->Release();

		(*outShader) = shader;

	}

	m_dstResource = outShader;
}