#pragma once 

#include"render/renderer.h"

#include "core/system.h"
#include "fwzSetup.h"

#include "utils/asset/AssetFactory.h"

namespace GkDemo
{
    class SchemaBuilder;
    class Demo;
    class Context;
}

#include "builtin_data/loaderbar.h"

class Application : public Grafkit::System, protected GrafkitData::LoaderBar
{
public:

    explicit Application(fwzSettings &settings);
    ~Application();

    int init() override;
    int mainloop() override;
    void release() override;


protected:
    void UpdateLoaderBar(float p) override;

private:
    std::unique_ptr<Grafkit::IAssetFactory> m_file_loader;

    Grafkit::Renderer m_render;
    std::unique_ptr<GkDemo::Context> m_context;
};

