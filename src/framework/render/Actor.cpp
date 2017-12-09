#include "stdafx.h"

#include "Actor.h"
#include "shader.h"
#include "scene.h"

using namespace Grafkit;

void Grafkit::Entity3D::_serialize(Archive & ar)
{
	this->IResource::_serialize(ar);
}

Grafkit::Entity3D::Entity3D() //: Grafkit::IResource()
{
}

Grafkit::Entity3D::~Entity3D()
{
}

// =================================================================

PERSISTENT_IMPL(Grafkit::Actor)

Grafkit::Actor::Actor() : Persistent(),
	m_viewMatrix(), m_transformMatrix(), m_ishidden(0)
{
}

Grafkit::Actor::Actor(Ref<Entity3D> entity) : Persistent(),
	m_viewMatrix(), m_transformMatrix(), m_ishidden(0)
{
	AddEntity(entity);
}

Grafkit::Actor::~Actor()
{
}

void Grafkit::Actor::Render(Grafkit::Renderer & render, Scene * scene)
{
	for (size_t i = 0; i < this->m_pEntities.size(); i++) {
		m_pEntities[i]->Render(render, scene);
	}
}

void Grafkit::Actor::AddChild(Actor* child)
{
	m_pChildren.push_back(child);
	child->m_pParent = this;
}

void Grafkit::Actor::serialize(Archive & ar)
{
	IResource::_serialize(ar);

	PERSIST_FIELD(ar, m_viewMatrix);
	PERSIST_FIELD(ar, m_transformMatrix);

}


// ================================================================

Grafkit::ActorEventHandler::ActorEventHandler()
{
}


Grafkit::ActorEventHandler::~ActorEventHandler()
{
}

void Grafkit::ActorEventHandler::PushShader(Scene * const & scene)
{
	otherPShader = scene->GetPShader();
	otherVShader = scene->GetPShader();
	
	if (myVShader.Invalid() || myPShader.Invalid())
		return;

	if (myVShader->Invalid() || myPShader->Invalid())
		return;

	scene->SetPShader(myPShader);
	scene->SetVShader(myVShader);

}

void Grafkit::ActorEventHandler::PopShader(Scene * const & scene)
{
	scene->SetPShader(otherPShader);
	scene->SetVShader(otherVShader);
}
