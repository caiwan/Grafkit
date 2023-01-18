#include "Light.h"

using namespace Grafkit;

BaseLight::BaseLight()
{
	ZeroMemory(&m_light, sizeof(m_light));
	m_light.la = 1.0;

	ZeroMemory(&m_position, sizeof(m_light));
	ZeroMemory(&m_direction, sizeof(m_light));
}

BaseLight::~BaseLight()
{
}

void BaseLight::SetShaderCB(ShaderRef &rPShader)
{
	m_light.type = this->GetLightType();

	rPShader["light"] = &m_light;
}

// ============================================================================================================