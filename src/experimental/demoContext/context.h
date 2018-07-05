#pragma once
#include "utils/ResourcePreloader.h"
#include "utils/InitializeSerializer.h"

namespace GkDemo
{
    class Demo;

    class Context : public Grafkit::ResourcePreloader, Grafkit::ClonableInitializer
    {
    public:

        explicit Context(Grafkit::Renderer &render, Grafkit::IAssetFactory* assetFactory);
        ~Context() override;

        // -> Editor
        /*void Intitialize();
        void Intitialize(Grafkit::IAssetRef document);
        void IntitializeFromSchema(Grafkit::IAssetRef schemaRef);

        void Save(std::string filename);
        void Load(Grafkit::IAssetRef assetRef);*/

        Demo* GetDemo() const { return m_demo; }
        void SetDemo(Demo* const demo) { m_demo = demo; }

        Grafkit::Renderer& GetDeviceContext() override { return m_render; }
        Grafkit::IAssetFactory* GetAssetFactory() override { return m_assetFactory; };

    private:
        Grafkit::Renderer& m_render;
        
        Demo* m_demo;

        Grafkit::IAssetFactory* m_assetFactory;
    };

    inline Context::Context(Grafkit::Renderer &render, Grafkit::IAssetFactory* assetFactory): ResourcePreloader()
        , ClonableInitializer()
        , m_render(render)
        , m_demo(nullptr)
        , m_assetFactory(assetFactory) {
    }

    inline Context::~Context() {
    }
 
}
