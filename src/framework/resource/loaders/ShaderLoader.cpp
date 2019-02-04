#include "stdafx.h"

#include "ShaderLoader.h"

#include "resource/ResourceManager.h"
#include "utils/asset//AssetFactory.h"

#include "core/resource.h"
#include "render/shader.h"


using namespace Grafkit;
using namespace FWdebugExceptions;

// =============================================================================================================================

namespace {
# if 0
    class IncludeProvider : public ID3DInclude {
    public:
        explicit IncludeProvider(IResourceManager* const & resman);
        virtual ~IncludeProvider() = default;

        HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;

        HRESULT __stdcall Close(LPCVOID pData) override;

    private:
        IResourceManager * const &m_resman;
    };

    IncludeProvider::IncludeProvider(IResourceManager* const& resman) : m_resman(resman) {
    }

    HRESULT IncludeProvider::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
    {
        std::string relpath;
        switch (IncludeType)
        {
        case D3D_INCLUDE_LOCAL:
            relpath = m_resman->GetResourcePath("shaderincludelocal") + pFileName;
            break;
        case D3D_INCLUDE_SYSTEM:
            relpath = m_resman->GetResourcePath("shaderincludesystem") + pFileName;
            break;

        default:
            return E_FAIL;
        }

        try
        {
            StreamRef asset = m_resman->GetAssetFactory()->Get(relpath);

            size_t assetLength = 0;
            StreamDataPtr assetData;
            asset->ReadAll(assetLength, assetData);
            // We'll need this 'little trick' in order to persist the memory when close is being called
            uint8_t* dst = new uint8_t[assetLength];
            memcpy(dst, assetData.get(), assetLength);
            *pBytes = assetLength;
            *ppData = dst;
        }
        catch (AssetLoadException& e)
        {
            LOGGER(Log::Logger().Error(e.what()));
            return E_FAIL;
        }
        return S_OK;
    }

    HRESULT IncludeProvider::Close(LPCVOID pData)
    {
        uint8_t* assetData = (uint8_t*)(pData);
        delete[]assetData;

        return S_OK;
    }

#endif 
}

// =============================================================================================================================
#if 0
void IShaderLoader::Load(IResourceManager * const & resman, IResource* source)
{
    if (m_params.entryPoint.empty())
        m_params.entryPoint = DefaultEntryPointName();

    ShaderResRef dstShader = static_cast<ShaderRes*>(source);
    if (dstShader.Invalid()) {
        return;
    }

    try {
        StreamRef asset = resman->GetAssetFactory()->Get(m_params.sourceName); //this->GetSourceAsset(resman);
        ShaderRef shader = NewShader();
        // load from asset

        LOGGER(Log::Logger().Info("Lading shader from resource %d %s@%s", shader->GetShaderType(), m_name.c_str(), m_params.entryPoint.c_str()));
        ID3DInclude * pInclude = new IncludeProvider(resman);

        size_t assetLength = 0;
        StreamDataPtr assetData = nullptr;
        asset->ReadAll(assetLength, assetData);
        shader->LoadFromMemory(resman->GetDeviceContext(), m_params.entryPoint.c_str(), LPCSTR(assetData.get()), assetLength, m_name.c_str(), pInclude, nullptr);
        delete pInclude;

        // 2.
        if (*dstShader) {
            (*dstShader)->Shutdown();
        }

        // 3.
        dstShader->AssingnRef(shader);

    }
    catch (std::exception &e) {
        // if we are about to replace an existing shader, but we had a compile error
        // then toss the error, due it was prompted to stodut
        // otherwise thow the ex
        if (!dstShader) {
            throw e;
        }
    }
}

void IShaderLoader::Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) {
    // Nothing to do ATM
}

#endif 

void IShaderLoader::Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

void IShaderLoader::Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

ShaderRef VertexShaderLoader::NewShader() const { return ShaderRef(new VertexShader()); }
ShaderRef PixelShaderLoader::NewShader() const { return ShaderRef(new PixelShader()); }
ShaderRef GeometryShaderLoader::NewShader() const { return ShaderRef(new GeometryShader()); }
