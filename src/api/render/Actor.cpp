#include "Actor.h"
#include "shader.h"

using namespace FWrender;

FWrender::Entity3D::Entity3D() : FWassets::IRenderAsset()
{
}

FWrender::Entity3D::~Entity3D()
{
}

// =================================================================

FWrender::Actor::Actor() : m_viewMatrix()
{
}

FWrender::Actor::~Actor()
{
	for (size_t j = 0; j < m_pChildren.size(); j++) {
		delete m_pChildren[j];
	}
}

void FWrender::Actor::Render(FWrender::Renderer & render)
{
	for (size_t i = 0; i < this->m_pEntities.size(); i++) {
		/// @todo: modelview update goez here - talan a kamerahoz kell hozzagyogyitani ezt?
		m_pEntities[i]->Render(render);
	}
}

void FWrender::Actor::AddChild(Actor * child)
{
	m_pChildren.push_back(child);
	child->m_pParent = this;
}
