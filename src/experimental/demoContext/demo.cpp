#include "demo.h"

#include "math/fbm.h"

#include "render/shader.h"
#include "render/effect.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"
#include "render/texture.h"

#include "render/SceneGraph.h"
#include "scene/scene.h"
#include "animation/actoranimation.h"

#include "resource/loaders/ShaderLoader.h"

#include "resource/ResourceManager.h"
#include "resource/loaders/MusicBassLoader.h"

#include "core/Music.h"

#include "schema.h"

#include "builtin_data/cube.h"

using namespace GkDemo;
using namespace Grafkit;
using namespace GrafkitData;

// ============================================================================================

Demo::Demo() : m_activeSceneId(0)
    , m_activeCameraId(0)
    , m_isFxaa(false)
{
    //m_renderTarget = new DeferredRenderTarget();

    m_mapNormal = new Texture2D();
    m_mapPosition = new Texture2D();
    m_mapRoughness = new Texture2D();
    m_mapMetallic = new Texture2D();
    m_mapEmission = new Texture2D();
}

Demo::~Demo() {
}

void Demo::Initialize(Renderer& render)
{
    for (size_t i = 0; i < GetSceneCount(); i++)
    {
        SceneResRef scene = GetScene(i);
        if (scene && *scene)
        {
            (*scene)->Initialize();
            (*scene)->Build(render, m_vs, m_ps);

            // camera - cameraId 
            size_t cameraCount = (*scene)->GetCameraCount();

            std::map<std::string, std::pair<uint32_t, uint32_t>> reversedIdPair;
            for (auto it : m_cameraIDs) { reversedIdPair[it.second.first] = it.first; }

            for (size_t j = 0; j < cameraCount; j++)
            {
                CameraRef camera = (*scene)->GetCamera(j);
                for (auto reversedIdPairIt : reversedIdPair) { if (0 == reversedIdPairIt.first.compare(camera->GetUuid())) { m_cameraIDs[reversedIdPairIt.second].second = j; } }
            }

            size_t animationCount = (*scene)->GetAnimationCount();
            for (size_t j = 0; j < animationCount; j++)
            {
                AnimationRef animation = (*scene)->GetAnimation(j);
                // -- snipity snapity
#if 0
                LOGGER(Log::Logger().Debug("Animation %d %s %p", j, animation->GetName().c_str(), animation.Get()));
                // --- 
                {
                    for (size_t k = 0; k < animation->GetTrackCount(); ++k)
                    {
                        Ref<Animation::Track> track = animation->GetTrack(k);
                        LOGGER(Log::Logger().Debug("- Track %s Ptr: %p", track->GetName().c_str(), track.Get()));

                        for (size_t l = 0; l < track->GetChannelCount(); ++l)
                        {
                            Ref<Animation::Channel> channel = track->GetChannel(l);
                            LOGGER(Log::Logger().Debug("-- Channel %s Ptr: %p", channel->GetName().c_str(), channel.Get()));
                        }
                    }
                }
                // --- 
#endif
                // -- snipity snapity
            }
        }
    }

    //m_renderTarget->Initialize(render);

    //m_mapNormal->InitializeFloatRGBA(render);
    //m_mapPosition->InitializeFloatRGBA(render);
    //m_mapRoughness->InitializeFloatRGBA(render);
    //m_mapMetallic->InitializeFloatRGBA(render);
    //m_mapEmission->InitializeFloatRGBA(render);

    m_effect = new EffectComposer();

    //m_effect->AddPass(new EffectPass(m_fxPBR));
    //m_effect->AddPass(new EffectPass(m_fxSSAO));
    //m_effect->AddPass(new EffectPass(m_fxSSAOBlur));
    //m_effect->AddPass(new EffectPass(m_fxBloom));
    //m_effect->AddPass(new EffectPass(m_fxChromaticAbberation));
    if (m_isFxaa) m_effect->AddPass(new EffectPass(m_fxaa));

    m_effect->Initialize(render);

    // Has to be texture Reference
    //m_effect->SetInput(1, m_mapNormal);
    //m_effect->SetInput(2, m_mapPosition);
    //m_effect->SetInput(3, m_mapRoughness);
    //m_effect->SetInput(4, m_mapMetallic);
    //m_effect->SetInput(5, m_mapEmission);
}


SceneResRef Demo::GetActiveScene() const { return m_scenes.at(m_activeSceneId); }

int Demo::PreRender(Renderer& render, float time)
{
    UpdateAnimations(time);

    SceneResRef scene = GetActiveScene();
    if (scene && *scene)
    {
        (*scene)->SetActiveCameraId(GetActiveCameraId());
        (*scene)->UpdateScene(render, time);
    }

    return 0;
}


int Demo::Render(Renderer& render, float time)
{
    // --- 
    render.BeginScene();

    //m_renderTarget->BindTarget();
    m_effect->BindInput(render);

    SceneResRef scene = GetActiveScene();
    if (scene && *scene) { (*scene)->Render(render); }

    m_effect->UnbindInput(render);

    m_effect->Render(render);

    render.EndScene();

    // ---
    return 0;
}

ShaderResRef Demo::GetPs() const { return m_ps; }
void Demo::SetPs(const ShaderResRef& resource) { m_ps = resource; }
ShaderResRef Demo::GetVs() const { return m_vs; }
void Demo::SetVs(const ShaderResRef& resource) { m_vs = resource; }

void Demo::SetActiveCameraId(const uint32_t activeCameraId)
{
    std::pair<uint32_t, uint32_t> id(m_activeSceneId, activeCameraId);
    auto it = m_cameraIDs.find(id);
    if (it != m_cameraIDs.end()) { m_activeCameraId = it->second.second; }
    else { m_activeCameraId = 0; }
}

MusicResRef Demo::GetMusic() const { return m_music; }

void Demo::SetMusic(const MusicResRef& resource) { m_music = resource; }

void Demo::AddScene(uint32_t id, const SceneResRef& ref)
{
    if (id >= m_scenes.size()) { fill_n(back_inserter(m_scenes), id - m_scenes.size() + 1, nullptr); }
    m_scenes[id] = ref;
}

void Demo::AddCameraId(uint32_t sceneId, std::string cameraUuid, uint32_t cameraId)
{
    std::pair<uint32_t, uint32_t> sceneCameraIdPair(sceneId, cameraId);
    std::pair<std::string, uint32_t> uuidLocalIdPair(cameraUuid, 0);
    m_cameraIDs[sceneCameraIdPair] = uuidLocalIdPair;
}

//

// ============================================================================================

//PERSISTENT_IMPL(GkDemo::DemoAnimation);

void DemoAnimation::Initialize()
{
    m_activeScene = new Track("ActiveScene");
    m_activeScene->CreateChannel("id");
    AddTrack(m_activeScene);

    m_activeCamera = new Track("ActiveCamera");
    m_activeCamera->CreateChannel("id");
    AddTrack(m_activeCamera);

    // some other terrible hacks goes here 
}

void DemoAnimation::Update(double time)
{
    if (m_target)
    {
        float fSceneId = floor(m_activeScene->GetChannel(0)->GetValue(time));
        m_target->SetActiveSceneId(static_cast<uint32_t>(fSceneId));

        float fCameraId = floor(m_activeCamera->GetChannel(0)->GetValue(time));
        m_target->SetActiveCameraId(uint32_t(fCameraId));
    }
}

//void DemoAnimation::Serialize(Archive& ar)
//{
//    Animation::_Serialize(ar);
//    m_activeScene = m_tracks[0];
//    m_activeCamera = m_tracks[1];
//}
