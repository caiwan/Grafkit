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

    class IShaderLoader : public ResourceBuilder<Shader, ShaderParams>
    {
    public:
        IShaderLoader() {
        }

        explicit IShaderLoader(const ShaderParams& params)
            : ResourceBuilder<Shader, ShaderParams>(params) {
        }

        IShaderLoader(const std::string& name, const std::string& uuid, const ShaderParams& params)
            : ResourceBuilder<Shader, ShaderParams>(name, uuid, params) {
        }

        void Load(IResourceManager* const& resman, IResource* source) override;
        void Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) override;

        // + Serialize maybe?

    protected:
        virtual Shader* NewShader() const = 0;
        virtual std::string DefaultEntryPointName() const = 0;
    };

    /**
     *
     */

    class VertexShaderLoader : public IShaderLoader
    {
    public:
        VertexShaderLoader() {
        }

        explicit VertexShaderLoader(const ShaderParams& params)
            : IShaderLoader(params) {
        }

        VertexShaderLoader(const std::string& name, const std::string& uuid, const ShaderParams& params)
            : IShaderLoader(name, uuid, params) {
        }

        // + Serialize maybe?


    protected:
        Shader * NewShader() const override;
        std::string DefaultEntryPointName() const override { return "mainVertex"; }
    };

    /**
    *
    */

    class PixelShaderLoader : public IShaderLoader
    {
    public:
        PixelShaderLoader() {
        }

        explicit PixelShaderLoader(const ShaderParams& params)
            : IShaderLoader(params) {
        }

        PixelShaderLoader(const std::string& name, const std::string& uuid, const ShaderParams& params)
            : IShaderLoader(name, uuid, params) {
        }

        // + Serialize maybe?


    protected:
        Shader * NewShader() const override;
        std::string DefaultEntryPointName() const override { return "mainPixel"; }
    };

    /**
    *
    */

    class GeometryShaderLoader : public IShaderLoader
    {
    public:
        GeometryShaderLoader() {
        }

        explicit GeometryShaderLoader(const ShaderParams& params)
            : IShaderLoader(params) {
        }

        GeometryShaderLoader(const std::string& name, const std::string& uuid, const ShaderParams& params)
            : IShaderLoader(name, uuid, params) {
        }

        // + Serialize maybe?


    protected:
        Shader * NewShader() const override;
        std::string DefaultEntryPointName() const override { return "mainGeometry"; }
    };
}
