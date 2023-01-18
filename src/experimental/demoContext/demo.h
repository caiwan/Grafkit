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
        virtual ~Demo();

        void Initialize(Grafkit::Renderer& render);

        int PreRender(Grafkit::Renderer& render, float time);
        int Render(Grafkit::Renderer& render, float time);

        // --- 

        Grafkit::SceneResRef GetActiveScene() const;

        Grafkit::ShaderResRef GetVs() const;
        void SetVs(const Grafkit::ShaderResRef& resource);

        Grafkit::ShaderResRef GetPs() const;
        void SetPs(const Grafkit::ShaderResRef& resource);

        uint32_t GetActiveSceneId() const { return m_activeSceneId; }
        void SetActiveSceneId(const uint32_t activeSceneId) { m_activeSceneId = activeSceneId; }

        uint32_t GetActiveCameraId() const { return m_activeCameraId; }
        void SetActiveCameraId(const uint32_t activeCameraId);

        Grafkit::MusicResRef GetMusic() const;
        void SetMusic(const Grafkit::MusicResRef& resource);

        void AddScene(uint32_t id, const Grafkit::SceneResRef& ref);
        size_t GetSceneCount() const { return m_scenes.size(); }

        void AddCameraId(uint32_t sceneId, std::string uuid, uint32_t cameraId);

        Grafkit::SceneResRef GetScene(size_t id) const { return m_scenes.at(id); }
    private:

        Grafkit::MusicResRef m_music;
        Grafkit::ShaderResRef m_vs, m_ps;
        std::vector<Grafkit::SceneResRef> m_scenes;

        //Grafkit::ShaderResRef m_psShowUv;
        Grafkit::EffectComposerRef m_effect;

        //std::vector<std::pair<uint32_t, std::string>> m_cameraIDs;
        std::map<std::pair<uint32_t, uint32_t>, std::pair<std::string, uint32_t>> m_cameraIDs; // sceneId, globalCameraId -> uuid, localCameraId

        uint32_t m_activeSceneId;
        uint32_t m_activeCameraId;
    };

    class DemoAnimation : public Grafkit::Animation
    {
        friend class Demo;
    public:
        DemoAnimation() {
        }

        void Initialize() override;
        void Update(double time) override;

        void SetTarget(const Ref<Demo>& target) { m_target = target; }
        Ref<Demo> GetTarget() const { return m_target; }

        PERSISTENT_DECL(GkDemo::DemoAnimation, 1);

    private:
        TrackRef m_activeScene;
        TrackRef m_activeCamera;
        Ref<Demo> m_target;

    protected:
        void Serialize(Grafkit::Archive& ar) override;
    };
}
