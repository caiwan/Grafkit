#pragma once
#include "core/system.h"
#include "render/renderer.h"
#include "resource/ResourceManager.h"

namespace Testing
{
    class TestApplicationContext : public Grafkit::System, public Grafkit::IResourceManager
    {
    public:
        explicit TestApplicationContext(Grafkit::Renderer& render, Grafkit::IAssetFactory* assetFactory);

        virtual ~TestApplicationContext();

        int Initialize() override;
        int Mainloop() override;
        void Release() override;


        Grafkit::Renderer& GetDeviceContext() override;
        Grafkit::IAssetFactory* GetAssetFactory() override;

    private:
        Grafkit::Renderer& m_render;
        Grafkit::IAssetFactory* m_assetFactory;
    };
}
