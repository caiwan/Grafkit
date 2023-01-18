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
}

Grafkit::ShaderLoader::ShaderLoader(Grafkit::IAssetRef resource, ShaderType_e type, ShaderResRef shaderasset)
	: Grafkit::IResourceBuilder(),
	m_resource(resource), m_in(shaderasset), m_type(type)
{
	m_entrypoint = default_entry_point_names[type];
}

Grafkit::ShaderLoader::~ShaderLoader()
{
	// nothing to do
}

void Grafkit::ShaderLoader::SetEntryPoint(std::string entrypoint)
{
	m_entrypoint = entrypoint;
}

///@todo a shader betoltes menjen on-the-fly, es ne repositorybol
void Grafkit::ShaderLoader::operator()(Grafkit::IResourceManager * const & assman)
{
	if (m_in.Invalid())
		throw EX(NullPointerException);
	//try 
	{
		ShaderRef shader = new Shader();
		// load from asset
		if (m_resource.Valid()) {
			LOG(TRACE) << "Lading shader from resource" << m_in->GetName() << "@" << m_entrypoint; // << m_in->GetUUID();
			shader->LoadFromMemory(assman->GetDeviceContext(), m_entrypoint.c_str(), (LPCSTR)m_resource->GetData(), m_resource->GetSize(), m_type);
		}
		else {
			///@todo load from compiled shader
			throw EX_DETAILS(NotImplementedMethodException, "Egyelore nem tamogatott a forrasbol valo shader betoltes");
		}

		// replace shader
		if (m_in->Valid())
			m_in->Release();

		(*m_in) = shader;

		// m_in->SetName(m_in->GetName() + ":" + m_entrypoint);

		// assman->AddObject(m_in);
	}
	//catch (MissingShaderException &e) {
	//	// olyan shadert toltottunk be valoszinuleg, aminek nincs meg az az entrypointja, ami eppen kellene
	//	// itt kivesszuk azt az assetmanagerbol, es az exceptiont eltesszuk
	//	
	//	assman->GetRepository(ROOT_REPOSITORY)->RemoveObject(m_in);
	//	//assman->RemoveObject(m_in);
	//}
	//catch (ShaderException &e) {
	//	// Itt tortent valami a shaderrel, vagy nem fordul le, vagy nincs ilyen entrypoint;
	//	// utobbi esetben nem ezeket a drodiokat keressuk
	//	assman->GetRepository(ROOT_REPOSITORY)->RemoveObject(m_in);
	//	//assman->RemoveObject(m_in);
	//}
}