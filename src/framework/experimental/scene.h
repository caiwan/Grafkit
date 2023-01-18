#pragma once

#include <list>
#include <vector>

#include "common.h"

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

        virtual ~EntityContainer() {
        }

        inline void AddEntiy(ActorRef node, Entity3DRef entity);
        size_t GetCount() const { return m_entities.size(); }
        ActorRef GetActor(size_t id) { return m_entities[id].m_actor; }
        inline ActorRef GetActor(std::string name);

        Entity3DRef GetEntity(size_t id) { return m_entities[id].m_entity; }
        inline Entity3DRef GetEntity(std::string name);

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

        virtual ~HasSceneGraphRole() {
        }

        void SetSceneGraph(SceneGraphRef& scenegraph) { m_scenegraph = scenegraph; }
        SceneGraphRef GetSceneGraph() const { return m_scenegraph; }

    protected:
        inline void InitializeSceneGraph();
        inline void ShutdownSceneGraph();

        inline void UpdateScenegraph();
        inline void RenderScenegraph(Renderer& render, CameraRef& camera);
        inline void BuildSceneGraph(Renderer& deviceContext);

        SceneGraphRef m_scenegraph;
    };

    // --- 

    class HasAnimationsRole
    {
    public:
        HasAnimationsRole() {
        }

        virtual ~HasAnimationsRole() {
        }

        void AddAnimation(AnimationRef animation) { m_animations.push_back(animation); }

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

        inline void AddCamera(ActorRef& actor, CameraRef& camera) const;

        size_t GetCameraId(std::string name) const { return m_cameras->GetId(name); }

        void SetActiveCameraId(size_t id) { m_activeCameraId = id; }
        size_t GetActiveCameraId() const { return m_activeCameraId; }

        ActorRef GetActiveCameraActor() const { return m_cameras->GetActor(m_activeCameraId); }
        CameraRef GetActiveCamera() const { return dynamic_cast<Camera*>(m_cameras->GetEntity(m_activeCameraId).Get()); }

        ActorRef GetCameraActor(std::string name) const { return m_cameras->GetActor(name); }
        ActorRef GetCameraActor(size_t id) const { return m_cameras->GetActor(id); }

        CameraRef GetCamera(std::string name) const { return dynamic_cast<Camera*>(m_cameras->GetEntity(name).Get()); }
        CameraRef GetCamera(size_t id) const { return dynamic_cast<Camera*>(m_cameras->GetEntity(id).Get()); }

        void AddCameraFrame(float time, std::string name);
        void AddCameraFrame(float time, size_t id);

    protected:
        void InitializeCameras();
        void ShutdownCameras();

        void UpdateCamera(Renderer& render) const;

        EntityContainer* m_cameras;
        size_t m_activeCameraId;
    };

    // --- 

    class HasLightsRole
    {
    public:
        void AddLight(ActorRef& actor, LightRef& camera) {
            //assert(0);
        }

    protected:

        void InitializeLights() {
            //assert(0);
        }

        void ShutdownLights() {
            //assert(0);
        }

        void UpdateLights() {
            //assert(0);
        }

        EntityContainer* m_lights;
    };


    // ---------------------------------------------------------------------------


    class Scene : public Object, 
        public HasSceneGraphRole,
        public HasAnimationsRole,
        public HasCamerasRole,
        public HasLightsRole
    {
    public:
        Scene();
        virtual ~Scene();

        void Initialize();
        void Shutdown();

        void Build(Renderer& render);
        void Build(Renderer& render, ShaderResRef vs, ShaderResRef ps); // legacy stuff

        void UpdateScene(Renderer& render, float time);

        void RenderFrame(Renderer& render, float time);
        void Render(Renderer& render);

        // -- persistent
        PERSISTENT_DECL(Grafkit::Scene, 1);
    protected:
        void Serialize(Archive& ar) override;
    };


    // ---------------------------------------------------------------------------
    // -> Demo CTX.
    class SceneRenderBuffers
    {
    public:
        //void Precalc(/* + resman */); // + resman

        // + Initialize
        // + Shutdown

        // + bind to render
        // + bind to shader param

    private:
        Texture2DResRef m_mapColor;
        Texture2DResRef m_mapNormal;
        Texture2DResRef m_mapPosition;
        Texture2DResRef m_mapHeight;
        Texture2DResRef m_mapRoughness;
        Texture2DResRef m_mapMetallic;
        Texture2DResRef m_mapEmission;
    };
}


// ---------------------------------------------------------------------------
// inline implementation
// ---------------------------------------------------------------------------

#include "EntityContainer.inl"

#include "hasScenegraphRole.inl"
#include "hasAnimationsRole.inl"
#include "hasCamerasRole.inl"
