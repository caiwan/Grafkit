#pragma once 

#include"render/renderer.h"

#include "core/system.h"
#include "fwzSetup.h"

#include "utils/AssetFactory.h"
#include "utils/ResourcePreloader.h"

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
    Grafkit::IAssetFactory * m_file_loader;
    GkDemo::Context *m_context;
    Ref<GkDemo::Demo> m_demo;

    Grafkit::Renderer m_render;
    Grafkit::MusicResRef m_music;
};

