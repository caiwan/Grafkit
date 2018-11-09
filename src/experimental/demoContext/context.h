#pragma once

#include "common.h"
#include "json_fwd.hpp"
#include "resource/ResourceManager.h"

using Json = nlohmann::json;

namespace Grafkit
{
    class IResourceBuilder;
}

namespace GkDemo
{
    class Demo;

    class Context : public Grafkit::IResourceManager
    {
    public:
        explicit Context(Grafkit::Renderer &render, const std::shared_ptr<Grafkit::IAssetFactory>& assetFactory);
        ~Context();

        Context(const Context& other) = delete;
        Context& operator=(const Context& other) = delete;

        Json LoadJson(const Grafkit::StreamRef & schemaAsset);

        void LoadDemo(const std::string filename = "demo.json");

        bool GetIsFxaa() const { return m_isFxaa; }
        void SetIsFxaa(const bool isFxaa) { m_isFxaa = isFxaa; }

        Ref<Demo> GetDemo() const;
        void SetDemo(const Ref<Demo> &demo);

        Grafkit::Renderer& GetDeviceContext() override { return m_render; }
        Grafkit::IAssetFactory* GetAssetFactory() override { return m_assetFactory.get(); }

        // TODO: -> editor
        void Relocate(std::string path);
        void SaveSchema(bool isAutoSave = false) const;

    protected:
        bool m_isFxaa;
        Grafkit::Renderer& m_render;
        Ref<Demo> m_demo;
        const std::shared_ptr<Grafkit::IAssetFactory> m_assetFactory;
        std::string m_myBasePath;

        std::map<std::string, std::function<Ref<Grafkit::IResourceBuilder>(const Json &)>> m_loaders;
    };
}
