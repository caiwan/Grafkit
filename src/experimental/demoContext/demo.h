#pragma once

#include "common.h"
#include "scene/renderable.h"
#include "scene/scene.h"

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

        // FX
        Grafkit::ShaderResRef GetFxPbr() const { return m_fxPBR; }
        void SetFxPbr(const Grafkit::ShaderResRef& fxPbr) { m_fxPBR = fxPbr; }
        Grafkit::ShaderResRef GetFxSsao() const { return m_fxSSAO; }
        void SetFxSsao(const Grafkit::ShaderResRef& fxSsao) { m_fxSSAO = fxSsao; }
        Grafkit::ShaderResRef GetFxSsaoBlur() const { return m_fxSSAOBlur; }
        void SetFxSsaoBlur(const Grafkit::ShaderResRef& fxSsaoBlur) { m_fxSSAOBlur = fxSsaoBlur; }
        Grafkit::ShaderResRef GetFxBloom() const { return m_fxBloom; }
        void SetFxBloom(const Grafkit::ShaderResRef& fxBloom) { m_fxBloom = fxBloom; }
        Grafkit::ShaderResRef GetFxChromaticAbberation() const { return m_fxChromaticAbberation; }
        void SetFxChromaticAbberation(const Grafkit::ShaderResRef& fxChromaticAbberation) { m_fxChromaticAbberation = fxChromaticAbberation; }
        Grafkit::ShaderResRef GetFxDofCompute() const { return m_fxDofCompute; }
        void SetFxDofCompute(const Grafkit::ShaderResRef& fxDofCompute) { m_fxDofCompute = fxDofCompute; }
        Grafkit::ShaderResRef GetFxDofBlur() const { return m_fxDofBlur; }
        void SetFxDofBlur(const Grafkit::ShaderResRef& fxDofBlur) { m_fxDofBlur = fxDofBlur; }
        Grafkit::ShaderResRef GetFxDofJoin() const { return m_fxDofJoin; }
        void SetFxDofJoin(const Grafkit::ShaderResRef& fxDofJoin) { m_fxDofJoin = fxDofJoin; }
        Grafkit::ShaderResRef GetFxaa() const { return m_fxaa; }
        void SetFxaa(const Grafkit::ShaderResRef& resource) { m_fxaa = resource; }

        bool GetIsFxaa() const { return m_isFxaa; }
        void SetIsFxaa(const bool isFxaa) { m_isFxaa = isFxaa; }

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

        Grafkit::ShaderResRef m_fxPBR;
        Grafkit::ShaderResRef m_fxSSAO;
        Grafkit::ShaderResRef m_fxSSAOBlur;
        Grafkit::ShaderResRef m_fxBloom;
        Grafkit::ShaderResRef m_fxChromaticAbberation;
        Grafkit::ShaderResRef m_fxDofCompute;
        Grafkit::ShaderResRef m_fxDofBlur;
        Grafkit::ShaderResRef m_fxDofJoin;
        Grafkit::ShaderResRef m_fxaa;

        Grafkit::Texture2DRef m_mapNormal;
        Grafkit::Texture2DRef m_mapPosition;
        Grafkit::Texture2DRef m_mapRoughness;
        Grafkit::Texture2DRef m_mapMetallic;
        Grafkit::Texture2DRef m_mapEmission;

        Grafkit::EffectComposerRef m_effect;

        std::map<std::pair<uint32_t, uint32_t>, std::pair<std::string, uint32_t>> m_cameraIDs; // sceneId, globalCameraId -> uuid, localCameraId

        uint32_t m_activeSceneId;
        uint32_t m_activeCameraId;

        bool m_isFxaa;
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
