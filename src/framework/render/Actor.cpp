#include "stdafx.h"

#include "Actor.h"
#include "shader.h"
#include "SceneGraph.h"

PERSISTENT_IMPL(Grafkit::Actor)

using namespace Grafkit;

Entity3D::Entity3D() :  Object(){

}

Entity3D::~Entity3D() {
}


void Entity3D::_Serialize(Archive& ar)
{
    Object::_Serialize(ar);
}

// =================================================================


Actor::Actor() : Object()
    , m_viewMatrix()
    , m_transformMatrix()
    , m_ishidden(0) 
{
}

Actor::Actor(Ref<Entity3D> entity) : Object()
    , m_viewMatrix()
    , m_transformMatrix()
    , m_ishidden(0)
{
    AddEntity(entity);
}

Actor::~Actor() {
}

void Actor::Render(Renderer& render, SceneGraph* scene) { for (size_t i = 0; i < this->m_pEntities.size(); i++) { m_pEntities[i]->Render(render, scene); } }

void Actor::AddChild(Actor* child)
{
    m_pChildren.push_back(child);
    child->m_pParent = this;
}

void Actor::Serialize(Archive& ar)
{
    _Serialize(ar);

    PERSIST_FIELD(ar, m_viewMatrix);
    PERSIST_FIELD(ar, m_transformMatrix);
}
