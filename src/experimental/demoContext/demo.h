#pragma once 
#include "render/renderer.h"
#include "render/effect.h"

namespace GkDemo
{

    class Demo
    {
    
    public:
        void Initialize();

        void PreRender(Grafkit::Renderer &render, float time);
        void Render(Grafkit::Renderer &render, float time);

    private:
        Grafkit::ShaderResRef m_vs, m_ps;
        Grafkit::SceneResRef m_scene;
        Grafkit::SceneGraphRef m_scenegraph;

        Grafkit::ShaderResRef m_psShowUv;
        Grafkit::EffectComposerRef m_effect;

        Grafkit::ActorRef m_rootActor;
        Grafkit::ActorRef m_cameraActor;

    };

}
