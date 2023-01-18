#include "Light.h"

Grafkit::BaseLight::BaseLight() : IResource()
{
	ZeroMemory(&m_light, sizeof(m_light));
	m_light.la = 1.0;
}

Grafkit::BaseLight::~BaseLight()
{
}

void Grafkit::BaseLight::SetShaderCB(ShaderRef &rPShader)
{
	m_light.type = this->GetLightType();

	// do the magic here
}

// ============================================================================================================