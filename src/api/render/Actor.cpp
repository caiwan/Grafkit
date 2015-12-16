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


void FWrender::Entity3D::setVertexShader(ShaderRef shader)
{
	if (shader.Valid() && shader->getShaderType() == ST_Vertex) {
		this->m_vertexShader = shader;
	}
	else {
		// thorw EX 
	}
}

void FWrender::Entity3D::setFragmentShader(ShaderRef shader)
{
	if (shader.Valid() && shader->getShaderType() == ST_Pixel) {
		this->m_vertexShader = shader;
	}
	else {
		// thorw EX 
	}
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
