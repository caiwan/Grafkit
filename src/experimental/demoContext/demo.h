#pragma once 

#include "common.h"
#include "render/renderer.h"

namespace Grafkit {
    class IResourceManager;
}

namespace GkDemo
{

    class Demo : public Referencable
    {
    
    public:
        Demo();
        ~Demo();

        void Preload(Grafkit::IResourceManager * const & resman);
        void Initialize(Grafkit::Renderer &render);

        int PreRender(Grafkit::Renderer &render, float time) const;
        int Render(Grafkit::Renderer& render, float time) const;

        Grafkit::SceneGraphRef GetScenegraph() const;
        Grafkit::SceneResRef GetScene() const;
        Grafkit::MusicResRef GetMusic() const;

    private:
        void InitTestStuff(Grafkit::Renderer & render);

        void InitAdvancedTestStuff(Grafkit::Renderer & render);


        Grafkit::MusicResRef m_music;

        Grafkit::ShaderResRef m_vs, m_ps;
        Grafkit::SceneResRef m_scene;
        Grafkit::SceneGraphRef m_scenegraph;

        Grafkit::ShaderResRef m_psShowUv;
        Grafkit::EffectComposerRef m_effect;

        Grafkit::ActorRef m_rootActor;
        Grafkit::ActorRef m_cameraActor;

    };

}
