#pragma once
#include "core/system.h"
#include "render/renderer.h"
#include "resource/ResourceManager.h"

namespace Testing
{
    class TestApplicationContext : public Grafkit::System
    {
    public:
        explicit TestApplicationContext(Grafkit::Renderer& render, std::unique_ptr<Grafkit::IAssetFactory> assetFactory);
        ~TestApplicationContext() {}

        int Initialize() override;
        int Mainloop() override;
        void Release() override;

        Grafkit::Renderer &GetRender() const { return m_render; }
        Grafkit::IAssetFactory &GetAssetFactory() const { return *(m_assetFactory.get()); }
        Grafkit::ResourceManager &GetResourceManager() const { return *(m_resourceManager.get()); }

    private:
        Grafkit::Renderer& m_render;
        std::unique_ptr<Grafkit::IAssetFactory> m_assetFactory;
        std::unique_ptr<Grafkit::ResourceManager> m_resourceManager;
    };
}
