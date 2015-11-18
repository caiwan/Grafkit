#include "Actor.h"
#include "shader.h"

using namespace FWrender;

FWrender::Entity3D::Entity3D()
{
	this->m_parent = NULL;
}

FWrender::Entity3D::~Entity3D()
{
}

void FWrender::Entity3D::setShader()
{
}

ShaderRef & FWrender::Entity3D::getShader()
{
	// TODO: insert return statement here
	return this->m_shader;
}

const Actor * FWrender::Entity3D::getParent()
{
	return this->m_parent;
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
