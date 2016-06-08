#include "../stdafx.h"

#include "Light.h"

using namespace Grafkit;

BaseLight::BaseLight()
{
	ZeroMemory(&m_light, sizeof(m_light));
	m_light.la = 1.0;

	ZeroMemory(&m_position, sizeof(m_position));
	ZeroMemory(&m_direction, sizeof(m_direction));
}

BaseLight::~BaseLight()
{
}

void BaseLight::SetShaderCB(ShaderRef &rPShader)
{
	m_light.type = this->GetLightType();

	//((Shader)(*rPShader))["light"] = &m_light;
	rPShader->GetParam("light").SetP(&m_light);
}

// ============================================================================================================