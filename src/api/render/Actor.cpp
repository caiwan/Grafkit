#include "Actor.h"
#include "shader.h"

using namespace FWrender;

FWrender::Entity3D::Entity3D() : FWassets::IRenderAsset()
{
	this->m_parent = NULL;
}

FWrender::Entity3D::~Entity3D()
{
}

// =================================================================

FWrender::Actor::Actor()
{
}

FWrender::Actor::Actor(const Actor &)
{
}

FWrender::Actor::~Actor()
{
}
