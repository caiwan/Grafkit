#pragma once
#include "resource/ResourceManager.h"
//#include "utils/ext/InitializeSerializer.h"
#include "schema.h"

namespace GkDemo
{
    class SchemaBuilder;
    class Demo;

    class Context : public Grafkit::IResourceManager
    {
        explicit Context(Grafkit::Renderer &render, Grafkit::IAssetFactory*const& assetFactory);
        ~Context();

        void Relocate(std::string path);

        void LoadScema();
        void SaveSchema(bool isAutoSave = false) const;

        void Intitialize();

        bool GetIsFxaa() const { return m_isFxaa; }
        void SetIsFxaa(const bool isFxaa) { m_isFxaa = isFxaa; }

        Ref<Demo> GetDemo() const;
        void SetDemo(const Ref<Demo> &demo);

        Grafkit::Renderer& GetDeviceContext() override { return m_render; }
        Grafkit::IAssetFactory* GetAssetFactory() override { return m_assetFactory; }

    protected:
        bool m_isFxaa;

        Grafkit::Renderer& m_render;

        Ref<Demo> m_demo;

        Grafkit::IAssetFactory* m_assetFactory;

        SchemaBuilder m_builder;
        std::string m_myBasePath;
    };

#if 0
    class Context : public Grafkit::ResourcePreloader, Grafkit::ClonableInitializer
    {
    public:

        explicit Context(Grafkit::Renderer &render, Grafkit::IAssetFactory* assetFactory);
        ~Context() override;

        void Relocate(std::string path);
        
        void LoadScema();
        void SaveSchema(bool isAutoSave = false) const;

        void Intitialize();


        bool GetIsFxaa() const { return m_isFxaa; }
        void SetIsFxaa(const bool isFxaa) { m_isFxaa = isFxaa; }

        Ref<Demo> GetDemo() const;
        void SetDemo(const Ref<Demo> &demo);

        Grafkit::Renderer& GetDeviceContext() override { return m_render; }
        Grafkit::IAssetFactory* GetAssetFactory() override { return m_assetFactory; }
        void CreateTestStuff();

    protected:
        Grafkit::Renderer& m_render;
        
        Ref<Demo> m_demo;

        Grafkit::IAssetFactory* m_assetFactory;

        SchemaBuilder m_builder;
        std::string m_myBasePath;
        //SchemaExport m_exporter;

        bool m_isFxaa;

        void SaveObject(const Ref<Grafkit::Object>& ref, const char* str, bool isAutoSave) const;
    };

#endif 

}
