#pragma once

#include "common.h"

#include "core/system.h"
#include "render/renderer.h"
#include "resource/ResourceManager.h"

namespace GkDemo
{
    class Demo;
    class Context;
}

namespace Grafkit
{
    class IAssetFactory;
    class IResourceManager;
}

namespace Testing
{
    class TestApplicationContext : public Grafkit::System
    {
    public:
        explicit TestApplicationContext();

        virtual ~TestApplicationContext();

        int init() override;
        int mainloop() override;

        void release() override;

        Grafkit::Renderer& GetRender() { return m_render; }
        GkDemo::Context& GetContext() const { return *m_context.get(); }
        std::shared_ptr<Grafkit::IAssetFactory> GetAssetFactory() const { return m_assetFactory; }

        template <class T>
        Ref<T> SafeGetObject(const char* uuid)
        {
            assert(uuid);
            Ref<Grafkit::Resource<T>> tResource = m_context->GetByUuid<Grafkit::Resource<T>>(uuid);
            if (!tResource)
                return nullptr;
            if (!*tResource)
                return nullptr;
            return tResource->Get();
        }

        template <class T>
        Ref<Grafkit::Resource<T>> SafeGetResource(const char* uuid)
        {
            assert(uuid);
            Ref<Grafkit::Resource<T>> tResource = m_context->GetByUuid<Grafkit::Resource<T>>(uuid);
            if (!tResource)
                return nullptr;
            return tResource;
        }

    private:
        Grafkit::Renderer m_render;
        std::unique_ptr<GkDemo::Context> m_context;
        std::shared_ptr<Grafkit::IAssetFactory>m_assetFactory;

    };
}
