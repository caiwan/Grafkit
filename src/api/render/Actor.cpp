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

	push();

	for (size_t i = 0; i < this->m_pChildren.size(); i++) {
		m_pChildren[i]->Render(render);
	}

	pop();
}

/// @todo nem jo, a bejarast a scene-be kell tenni, nem ide :C

void FWrender::Actor::push()
{
	m_matrixStack.push()
}

void FWrender::Actor::pop()
{
}

