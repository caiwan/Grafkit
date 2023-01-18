#pragma once

#include <vector>

#include "../utils/reference.h"

#include "../math/matrix.h"

#include "renderer.h"


#include "../utils/persistence/persistence.h"

namespace Grafkit {

    class SceneGraph;

    class Actor;

    class Entity3D : public Persistent, virtual public Referencable
    {
    protected:

        friend class Actor;
    public:
        Entity3D();
        virtual ~Entity3D();

        virtual void Calculate(Renderer& renderer, ActorRef parent = nullptr) = 0;
        virtual void Render(Renderer& deviceContext, SceneGraph * const & scene) = 0;
        virtual void Build(Renderer& deviceContext, SceneGraph * const & scene) = 0;

        std::string GetName() const { return this->m_name; }
        void SetName(std::string name) { m_name = name; }

    protected:
        std::string m_name;

        void Serialize(Archive& ar) override = 0;
        void _Serialize(Archive& ar);
    };


    /**
    An actor node - ez a scenegraph es a nodeja
    */
    __declspec(align(16))
        class Actor : public AlignedNew<Actor>, public Persistent, virtual public Referencable
    {
        friend class SceneGraph;
    public:
        Actor();
        explicit Actor(Ref<Entity3D> entity);
        ~Actor();

        // kisse kulonos megoldas ... 
        void Matrix(const Matrix &mat) { m_viewMatrix = mat; }
        void Transform(const Grafkit::Matrix &mat) { m_transformMatrix = mat; }

        Grafkit::Matrix& Matrix() { return m_viewMatrix; }
        Grafkit::Matrix& Transform() { return m_transformMatrix; }

        virtual void Render(Renderer &render, SceneGraph* scene);

        /// @todo igazi ListTree-t hasznaljon, ha lehet, es majd mukodik
        Ref<Actor> GetParent() const { return m_pParent; }

        void AddChild(Actor* child);
        ActorRef GetChild(int i = 0) { return this->m_pChildren[i]; }
        size_t GetChildrenCount() const { return this->m_pChildren.size(); }

        void AddEntity(Ref<Entity3D> entity) { m_pEntities.push_back(entity); }
        Entity3DRef GetEntity(int id = 0) { return m_pEntities[id]; }
        size_t GetEntityCount() const { return m_pEntities.size(); }

        void Hide() { m_ishidden = 1; }
        void Show() { m_ishidden = 0; }

        int IsHidden() const { return (m_pParent.Invalid() ? 0 : m_pParent->IsHidden()) || m_ishidden; }

        // calculated matrix for internal use
        Grafkit::Matrix WorldMatrix() const { return m_worldMatrix; }

        std::string GetName() const { return this->m_name; }
        void SetName(std::string name) { m_name = name; }


    protected:
        std::string m_name;

        void WorldMatrix(const Grafkit::Matrix &mat) { m_worldMatrix = mat; }

        Grafkit::Matrix m_viewMatrix;			///< Node tranyyformacioja
        Grafkit::Matrix m_transformMatrix;		///< Kulon transzformacio a node tetejen (hogy ne legyen szukseg az eredeti matrixra)
        Grafkit::Matrix m_worldMatrix;			///< Szarmaztatott matrix

        Ref<Actor> m_pParent;
        std::vector<Ref<Actor>> m_pChildren;
        std::vector<Ref<Entity3D>> m_pEntities;

        int m_ishidden;

        PERSISTENT_DECL(Grafkit::Actor, 1);
    protected:
        void Serialize(Archive& ar) override;
    };

}

