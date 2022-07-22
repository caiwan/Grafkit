#pragma once

#include <list>
#include <vector>

#include "common.h"

#include "renderable.h"


#include "render/SceneGraph.h"

#include "render/Shader.h"
#include "render/Camera.h"
#include "render/Light.h"

namespace Grafkit
{
    class SceneRenderBuffers;
    class CameraContainer;
    class LightContainer;

    class Scene;

    typedef Ref<Scene> SceneRef;
    typedef Resource<Scene> SceneRes;
    typedef Ref<SceneRes> SceneResRef;

    class EntityContainer
    {
    public:
        EntityContainer() {
        }

        inline void AddEntiy(ActorRef node, Entity3DRef entity);
        size_t GetCount() const { return m_entities.size(); }

        ActorRef GetActor(size_t id) const { return m_entities.at(id).m_actor; }
        inline ActorRef GetActor(std::string name) const;

        Entity3DRef GetEntity(size_t id) const { return m_entities.at(id).m_entity; }
        inline Entity3DRef GetEntity(std::string name) const;

        inline size_t GetId(std::string name);

        inline void Calculate(Renderer& render, size_t id);
        inline void CalculateAll(Renderer& render);

    protected:
        struct entity_t
        {
            entity_t() : m_id(0) {
            }

            entity_t(ActorRef& actor, Entity3DRef& entity, size_t id) : m_actor(actor)
                , m_entity(entity)
                , m_id(id) {
            }

            ActorRef m_actor;
            Entity3DRef m_entity;
            size_t m_id;
        };

        std::vector<entity_t> m_entities;
        std::map<std::string, entity_t> m_entityMap;
    };

    // --- 
    // Separate varoius responsibilites of a monolyth class
    class HasSceneGraphRole
    {
    public:
        HasSceneGraphRole() {
        }

        void SetSceneGraph(const SceneGraphRef& scenegraph) { m_scenegraph = scenegraph; }
        SceneGraphRef GetSceneGraph() const { return m_scenegraph; }

    protected:
        inline void InitializeSceneGraph();

        inline void UpdateScenegraph();
        inline void RenderScenegraph(Renderer& render, const CameraRef& camera);
        inline void BuildSceneGraph(Renderer& deviceContext);

        SceneGraphRef m_scenegraph;
    };

    // --- 

    class HasAnimationsRole
    {
    public:
        HasAnimationsRole() {
        }

        void AddAnimation(const AnimationRef &animation) { m_animations.push_back(animation); }

        size_t GetAnimationCount() const { return m_animations.size(); }
        AnimationRef GetAnimation(size_t id) const { return m_animations.at(id); }

    protected:
        void UpdateAnimations(float t);
        std::vector<AnimationRef> m_animations;
    };

    // --- 

    class HasCamerasRole
    {
    public:
        HasCamerasRole();
        virtual ~HasCamerasRole();

        inline void AddCamera(const ActorRef& actor, const CameraRef& camera) const;

        size_t GetCameraId(const std::string & name) const { return m_cameras->GetId(name); }

        size_t GetCameraCount() const { return m_cameras->GetCount(); }

        void SetActiveCameraId(size_t id) { m_activeCameraId = id; }
        size_t GetActiveCameraId() const { return m_activeCameraId; }

        ActorRef GetActiveCameraActor() const { return m_cameras->GetActor(m_activeCameraId); }
        CameraRef GetActiveCamera() const { return std::dynamic_pointer_cast<Camera>(m_cameras->GetEntity(m_activeCameraId)); }

        ActorRef GetCameraActor(const std::string & name) const { return m_cameras->GetActor(name); }
        ActorRef GetCameraActor(size_t id) const { return m_cameras->GetActor(id); }

        CameraRef GetCamera(const std::string & name) const { return std::dynamic_pointer_cast<Camera>(m_cameras->GetEntity(name)); }
        CameraRef GetCamera(size_t id) const { return std::dynamic_pointer_cast<Camera>(m_cameras->GetEntity(id)); }

    protected:
        void UpdateCamera(Renderer& render) const;

        std::unique_ptr<EntityContainer> m_cameras;
        size_t m_activeCameraId;
    };

    // --- 

    class HasLightsRole
    {
    public:

        HasLightsRole();

        void AddLight(const ActorRef& actor, const LightRef& light);

        size_t GetLightCount() const { return m_lights->GetCount(); };

        LightRef GetLight(size_t id) const { return std::dynamic_pointer_cast<Light>(m_lights->GetEntity(id)); }
        LightRef GetLight(std::string name) const { return std::dynamic_pointer_cast<Light>(m_lights->GetEntity(name)); }

        ActorRef GetLightActor(size_t id) const { return m_lights->GetActor(id); }
        ActorRef GetLightActor(std::string name) const { return m_lights->GetActor(name); }

        size_t GetLightId(std::string name) const { return m_lights->GetId(name); }

    protected:
        inline void UpdateLight(Renderer& render, size_t id);
        inline void UpdateLights(Renderer& render);

        std::unique_ptr<EntityContainer> m_lights;
    };


    // ---------------------------------------------------------------------------


    class Scene : public Object, public Renderable,
        public HasSceneGraphRole,
        public HasAnimationsRole,
        public HasCamerasRole,
        public HasLightsRole
    {
    public:
        Scene();

        void Initialize() override;

        void Build(Renderer& render) override;
        void Build(Renderer& render, ShaderResRef vs, ShaderResRef ps) override; // legacy stuff

        void UpdateScene(Renderer& render, float time) override;

        void RenderFrame(Renderer& render, float time) override;
        void Render(Renderer& render) override;

        // -- persistent
        PERSISTENT_DECL(Grafkit::Scene, 1);
    };
}


// =============================================================================================
// inline implementations
// =============================================================================================

#include "EntityContainer.inl"

// =============================================================================================

inline void Grafkit::HasSceneGraphRole::InitializeSceneGraph()
{
    assert(m_scenegraph);
    m_scenegraph->Initialize();
}

inline void Grafkit::HasSceneGraphRole::UpdateScenegraph()
{
    assert(m_scenegraph);
    m_scenegraph->Update();
}

inline void Grafkit::HasSceneGraphRole::RenderScenegraph(Renderer & render, const CameraRef & camera)
{
    assert(m_scenegraph);
    m_scenegraph->Render(render, camera);
}

inline void Grafkit::HasSceneGraphRole::BuildSceneGraph(Renderer & render)
{
    assert(m_scenegraph);
    m_scenegraph->BuildScene(render);
}

// =============================================================================================

inline Grafkit::HasCamerasRole::HasCamerasRole() : m_cameras(std::make_unique<EntityContainer>())
, m_activeCameraId(0) {
}

inline void Grafkit::HasCamerasRole::AddCamera(const ActorRef& actor, const CameraRef& camera) const { m_cameras->AddEntiy(actor, camera); }

inline Grafkit::HasCamerasRole::~HasCamerasRole() {
}

inline void Grafkit::HasCamerasRole::UpdateCamera(Renderer& render) const { m_cameras->Calculate(render, m_activeCameraId); }

// =============================================================================================

inline Grafkit::HasLightsRole::HasLightsRole() : m_lights(std::make_unique<EntityContainer>()) {
}

inline void Grafkit::HasLightsRole::AddLight(const ActorRef& actor, const LightRef& light) { m_lights->AddEntiy(actor, light); }

inline void Grafkit::HasLightsRole::UpdateLight(Renderer& render, size_t id) {
    m_lights->Calculate(render, id);
}

inline void Grafkit::HasLightsRole::UpdateLights(Renderer& render) {
    m_lights->CalculateAll(render);
}

