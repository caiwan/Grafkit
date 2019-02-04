#pragma once

#include "common.h"
#include "resource/ResourceBuilder.h"

namespace Grafkit
{
    struct ShaderParams
    {
        std::string sourceName;
        std::string entryPoint;
    };

    class IShaderLoader : public ResourceLoader<Shader, ShaderParams>
    {
    public:

        IShaderLoader(const Uuid& id, const ShaderParams& params)
            : ResourceLoader<Shader, ShaderParams>(id, params) {
        }

    protected:
        void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
        void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;

        virtual ShaderRef NewShader() const = 0;
        virtual std::string DefaultEntryPointName() const = 0;
    };

    /**
     * VertexShaderLoader
     */

    class VertexShaderLoader : public IShaderLoader
    {
    public:
        VertexShaderLoader(const Uuid& id, const ShaderParams& params)
            : IShaderLoader(id, params) {
        }

    protected:
        ShaderRef NewShader() const override;
        std::string DefaultEntryPointName() const override { return "mainVertex"; }
    };

    /**
    * PixelShaderLoader
    */

    class PixelShaderLoader : public IShaderLoader
    {
    public:
        PixelShaderLoader(const Uuid& id, const ShaderParams& params)
            : IShaderLoader(id, params) {
        }

    protected:
        ShaderRef NewShader() const override;
        std::string DefaultEntryPointName() const override { return "mainPixel"; }
    };

    /**
    * GeometryShaderLoader
    */

    class GeometryShaderLoader : public IShaderLoader
    {
    public:
        GeometryShaderLoader(const Uuid& id, const ShaderParams& params)
            : IShaderLoader(id, params) {
        }

    protected:
        ShaderRef NewShader() const override;
        std::string DefaultEntryPointName() const override { return "mainGeometry"; }
    };
}
