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

        void LoadDemo(const std::string filename = "demo.json");

        bool GetIsFxaa() const { return m_isFxaa; }
        void SetIsFxaa(const bool isFxaa) { m_isFxaa = isFxaa; }

        Ref<Grafkit::Resource<Demo>> GetDemo() const;
        void SetDemo(const Ref<Grafkit::Resource<Demo>> &demo);

        Grafkit::Renderer& GetDeviceContext() override { return m_render; }
        Grafkit::IAssetFactory* GetAssetFactory() override { return m_assetFactory.get(); }

        // TODO: -> editor
        void Relocate(std::string path);
        void SaveSchema(bool isAutoSave = false) const;

    private:
        Json LoadJson(const Grafkit::StreamRef & schemaAsset);

    protected:
        bool m_isFxaa;
        Grafkit::Renderer& m_render;
        const std::shared_ptr<Grafkit::IAssetFactory> m_assetFactory;
        std::string m_myBasePath;

        Ref<Grafkit::Resource<Demo>> m_demo;

        std::map<std::string, std::function<Ref<Grafkit::IResourceBuilder>(const Json &)>> m_loaders;
    };
}
