#include "Light.h"

FWrender::BaseLight::BaseLight() : IRenderAsset()
{
	ZeroMemory(&m_light, sizeof(m_light));
	m_light.la = 1.0;
}

FWrender::BaseLight::~BaseLight()
{
}

void FWrender::BaseLight::SetShaderCB(ShaderRef rPShader)
{
	m_light.type = this->GetLightType();

	// do the magic here
}

// ============================================================================================================