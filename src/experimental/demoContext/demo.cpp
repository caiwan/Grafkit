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

Demo::~Demo() { //(*m_music)->Stop();
}

void Demo::Preload(IResourceManager* const& resman)
{
    assert(0);
}

void Demo::Initialize(Renderer& render)
{
    AddAnimation(new DemoAnimation());

    for (size_t i = 0; i < GetSceneCount(); i++)
    {
        SceneResRef scene = GetScene(i);
        if (scene && *scene)
        {
            (*scene)->Initialize();
            (*scene)->Build(render, m_vs, m_ps);
        }
    }

    for (size_t i = 0; i < GetAnimationCount(); ++i)
    {
        AnimationRef animation = GetAnimation(i);
        animation->Initialize();
    }
}

SceneResRef Demo::GetActiveScene() const { return m_scenes.at(m_activeSceneId); }

int Demo::PreRender(Renderer& render, float time) const { return 0; }


int Demo::Render(Renderer& render, float time) const
{
    // --- 
    render.BeginScene();

    float& t = time;

    SceneResRef scene = GetActiveScene();
    if (scene) { (*scene)->RenderFrame(render, t); }

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

// This thing serves testing purposes for the editor.
void Demo::InitTestStuff(Renderer& render)
{
    assert(0);
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
