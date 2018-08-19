#pragma once
#include "utils/ResourcePreloader.h"
#include "utils/InitializeSerializer.h"
#include "schema.h"

namespace GkDemo
{
    class SchemaBuilder;
    class Demo;

    class Context : public Grafkit::ResourcePreloader, Grafkit::ClonableInitializer
    {
    public:

        explicit Context(Grafkit::Renderer &render, Grafkit::IAssetFactory* assetFactory);
        ~Context() override;

        void Relocate(std::string path);
        
        void LoadScema();
        void SaveSchema(bool isAutoSave = false) const;

        void Intitialize();

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

        void SaveObject(const Ref<Grafkit::Object>& ref, const char* str, bool isAutoSave) const;
    };


}
