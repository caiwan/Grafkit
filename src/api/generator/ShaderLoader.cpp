#include "stdafx.h"

#include "ShaderLoader.h"

#include "../utils/ResourceBuilder.h"
#include "../utils/AssetFactory.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

// =============================================================================================================================

typedef Resource<IAsset> IAssetRes;
typedef Ref<IAssetRes> IAssetResRef;

class IncludeProvider : public ID3DInclude {
public:

	IncludeProvider(IResourceManager * const & resman) : m_resman(resman)
	{
	}

	HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) {
		IAssetRef asset;
		std::string relpath;
		switch (IncludeType) {
		case D3D_INCLUDE_LOCAL:
			relpath = m_resman->GetResourcePath("shaderincludelocal") + pFileName;
			break;
		case D3D_INCLUDE_SYSTEM:
			relpath = m_resman->GetResourcePath("shaderincludesystem") + pFileName;
			break;

		default:
			return E_FAIL;
		}

		try {
			asset = m_resman->GetAssetFactory()->Get(relpath);
			*ppData = asset->GetData();
			*pBytes = asset->GetSize();

			// -- resmanhoz hozzaad
			char buffer[256];
			sprintf_s(buffer, "pointer@%d", asset->GetData());
			IAssetResRef assetRes = new IAssetRes(asset);
			assetRes->SetName(buffer);
			m_resman->Add(assetRes);

		}
		catch (AssetLoadException &e) {
			LOGGER(Log::Logger().Error(e.what()));
			return E_FAIL;
		}
		return S_OK;
	}

	HRESULT __stdcall Close(LPCVOID pData) {
		// -- resmanbol kiszed
		char buffer[256];
		sprintf_s(buffer, "pointer@%d", pData);
		m_resman->Remove(buffer);

		return S_OK;
	}

private:
	IResourceManager * const &m_resman;
};

// =============================================================================================================================

Grafkit::ShaderLoader::ShaderLoader(std::string name, std::string sourcename, std::string entrypoint) : Grafkit::IResourceBuilder(name, sourcename),
m_entrypoint(entrypoint)
{
}

Grafkit::ShaderLoader::~ShaderLoader()
{
}

void Grafkit::ShaderLoader::Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source)
{
	if (m_entrypoint.empty())
		m_entrypoint = DefaultEntryPointName();
	
	ShaderResRef dstSahder = (ShaderRes*)source;
	if (dstSahder.Invalid()) {
		return;
	}
	
	Grafkit::IAssetRef asset = this->GetSourceAsset(resman);
	ShaderRef shader = NewShader();
	// load from asset
	if (asset.Valid()) {
		LOGGER(Log::Logger().Info("Lading shader from resource %d %s@%s", shader->GetShaderType(), m_name.c_str(), m_entrypoint.c_str()));
		ID3DInclude * pInclude= new IncludeProvider(resman);
		shader->LoadFromMemory(resman->GetDeviceContext(), m_entrypoint.c_str(), (LPCSTR)asset->GetData(), asset->GetSize(), m_name.c_str(), pInclude, nullptr);
		delete pInclude;
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
