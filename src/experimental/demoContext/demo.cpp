#include "demo.h"

#include "math/fbm.h"

#include "render/shader.h"
#include "render/effect.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"
#include "render/texture.h"

#include "render/SceneGraph.h"
#include "experimental/scene.h"
#include "animation/actoranimation.h"

#include "generator/ShaderLoader.h"

#include "utils/ResourceManager.h"
#include "generator/MusicBassLoader.h"

#include "core/Music.h"

#include "schema.h"

#include "builtin_data/cube.h"

using namespace GkDemo;
using namespace Grafkit;
using namespace GrafkitData;

// ============================================================================================

Demo::Demo(): m_activeSceneId(0) {
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

            size_t animationCount = (*scene)->GetAnimationCount();
            for (size_t j = 0; j< animationCount; j++)
            {
                AnimationRef animation = (*scene)->GetAnimation(j);
                // -- snipity snapity
#if 1
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
}

SceneResRef Demo::GetActiveScene() const { return m_scenes.at(m_activeSceneId); }

int Demo::PreRender(Renderer& render, float time)
{
    UpdateAnimations(time);

    SceneResRef scene = GetActiveScene();
    if (scene && *scene) { (*scene)->UpdateScene(render, time); }

    return 0;
}


int Demo::Render(Renderer& render, float time)
{
    // --- 
    render.BeginScene();

    SceneResRef scene = GetActiveScene();
    if (scene && *scene) { (*scene)->Render(render); }

    render.EndScene();

    // ---
    return 0;
}

ShaderResRef Demo::GetPs() const { return m_ps; }
void Demo::SetPs(const ShaderResRef& resource) { m_ps = resource; }
ShaderResRef Demo::GetVs() const { return m_vs; }
void Demo::SetVs(const ShaderResRef& resource) { m_vs = resource; }

MusicResRef Demo::GetMusic() const { return m_music; }

void Demo::SetMusic(const MusicResRef& resource) { m_music = resource; }

void Demo::AddScene(uint32_t id, const SceneResRef& ref)
{
    if (id >= m_scenes.size()) { fill_n(back_inserter(m_scenes), id - m_scenes.size() + 1, nullptr); }
    m_scenes[id] = ref;
}

// ============================================================================================

PERSISTENT_IMPL(GkDemo::DemoAnimation);

void DemoAnimation::Initialize()
{
    m_activeScene = new Track("ActiveScene");
    m_activeScene->CreateChannel("id");
    AddTrack(m_activeScene);
}

void DemoAnimation::Update(double time)
{
    if (m_target)
    {
        float id = floor(m_activeScene->GetChannel(0)->GetValue(time));
        m_target->SetActiveSceneId(static_cast<uint32_t>(id));
        // ... 
    }
}

void DemoAnimation::Serialize(Archive& ar) {
}
