#pragma once

#include <vector>

#include "common.h"
#include "renderer.h"
#include "math/matrix.h"

namespace Grafkit
{
    class SceneGraph;

    class Actor;

    class Entity3D : public Serializable
    {
    protected:

        friend class Actor;
    public:
        Entity3D() = default;
        virtual ~Entity3D() = default;

        virtual void Calculate(Renderer& renderer, const ActorRef& parent = nullptr) = 0;
        virtual void Render(Renderer& deviceContext, SceneGraph& scene) = 0;
        virtual void Build(Renderer& deviceContext, SceneGraph& scene) = 0;

        std::string GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

    protected:
        template <class Archive>
        void Serialize(Archive& ar)
        {
            ar & m_name;
        }

        std::string m_name;
    };

    /**
    */
    __declspec(align(16))
    class Actor : public AlignedNew<Actor>, public Object
    {
        friend class SceneGraph;
    public:
        Actor();
        explicit Actor(const Entity3DRef& entity);
        ~Actor();

        // kisse kulonos megoldas ... 
        void Matrix(const Matrix& mat) { m_viewMatrix = mat; }
        void Transform(const Grafkit::Matrix& mat) { m_transformMatrix = mat; }

        Grafkit::Matrix& Matrix() { return m_viewMatrix; }
        Grafkit::Matrix& Transform() { return m_transformMatrix; }

        virtual void Render(Renderer& render, SceneGraph& scene);

        /// @todo igazi ListTree-t hasznaljon, ha lehet, es majd mukodik
        //ActorRef GetParent() const { return m_pParent; }

        void AddChild(const ActorRef& child);
        ActorRef GetChild(int i = 0) { return this->m_pChildren[i]; }
        size_t GetChildrenCount() const { return this->m_pChildren.size(); }

        void AddEntity(const Entity3DRef& entity) { m_pEntities.push_back(entity); }
        Entity3DRef GetEntity(int id = 0) { return m_pEntities[id]; }
        size_t GetEntityCount() const { return m_pEntities.size(); }

        void Hide() { m_isHidden = true; }
        void Show() { m_isHidden = false; }

        //bool IsHidden() const { return (m_pParent ? 0 : m_pParent->IsHidden()) || m_isHidden; }
        bool IsHidden() const { return m_isHidden; }

        // calculated matrix for internal use
        Grafkit::Matrix WorldMatrix() const { return m_worldMatrix; }

        std::string GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

    protected:

        void WorldMatrix(const Grafkit::Matrix& mat) { m_worldMatrix = mat; }

        Grafkit::Matrix m_viewMatrix;
        Grafkit::Matrix m_transformMatrix;
        Grafkit::Matrix m_worldMatrix;

        //ActorRef m_pParent;
        std::vector<ActorRef> m_pChildren;
        std::vector<Entity3DRef> m_pEntities;

        bool m_isHidden;

        std::string m_name;

        SERIALIZE(Grafkit::Actor, 1, ar)
        {
            Object::Serialize(ar);
            ar & m_viewMatrix & m_transformMatrix & m_isHidden;
        }
    };
}
