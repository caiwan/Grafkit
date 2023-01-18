#pragma once

#include "common.h"
#include "render/renderer.h"
#include "experimental/scene.h"

namespace Grafkit
{
    class IResourceManager;
}

namespace GkDemo
{
    class DemoAnimation;
    typedef Ref<DemoAnimation> SceneAnimationRef;


    class Demo : public Referencable, public Grafkit::HasAnimationsRole
    {
    public:
        Demo();
        ~Demo();

        void Preload(Grafkit::IResourceManager* const & resman);
        void Initialize(Grafkit::Renderer& render);

        int PreRender(Grafkit::Renderer& render, float time) const;
        Grafkit::SceneResRef GetActiveScene() const;
        int Render(Grafkit::Renderer& render, float time) const;

        Grafkit::ShaderResRef GetVs() const;
        void SetVs(const Grafkit::ShaderResRef& resource);

        Grafkit::ShaderResRef GetPs() const;
        void SetPs(const Grafkit::ShaderResRef& resource);

        uint32_t GetActiveSceneId() const { return m_activeSceneId; }
        void SetActiveSceneId(const uint32_t activeSceneId) { m_activeSceneId = activeSceneId; }

        Grafkit::MusicResRef GetMusic() const;

        void AddScene(uint32_t id, const Grafkit::SceneResRef& ref);
        size_t GetSceneCount() const { return m_scenes.size(); }
        Grafkit::SceneResRef GetScene(size_t id) const { return m_scenes.at(id); }

    private:
        void InitTestStuff(Grafkit::Renderer& render);


        Grafkit::MusicResRef m_music;

        Grafkit::ShaderResRef m_vs, m_ps;

        std::vector<Grafkit::SceneResRef> m_scenes;

        //Grafkit::ShaderResRef m_psShowUv;
        Grafkit::EffectComposerRef m_effect;

        uint32_t m_activeSceneId;
    };

    class DemoAnimation : public Grafkit::Animation
    {
        friend class Demo;
    public:
        DemoAnimation() {
        }

        void Initialize() override;
        void Update(double time) override;

        PERSISTENT_DECL(GkDemo::DemoAnimation, 1);

    private:
        TrackRef m_activeScene;
        Ref<Demo> m_target;

    protected:
        void Serialize(Grafkit::Archive& ar) override;
    };
}
