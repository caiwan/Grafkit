#include "Actor.h"
#include "shader.h"

using namespace Grafkit;

Grafkit::Entity3D::Entity3D() //: Grafkit::IResource()
{
}

Grafkit::Entity3D::~Entity3D()
{
}

// =================================================================

Grafkit::Actor::Actor() : m_viewMatrix()
{
}

Grafkit::Actor::~Actor()
{
	for (size_t j = 0; j < m_pChildren.size(); j++) {
		delete m_pChildren[j];
	}
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

