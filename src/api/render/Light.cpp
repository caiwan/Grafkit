#include "stdafx.h"

#include "Light.h"

#include "Scene.h"

using namespace Grafkit;

Light::Light() : Entity3D()
{
	ZeroMemory(&m_light, sizeof(m_light));
	m_light.la = 1.0;
}

Light::~Light()
{
}

//void Light::SetShaderCB(ShaderRef &rPShader)
//{
//	m_light.type = this->GetLightType();
//
//	//((Shader)(*rPShader))["light"] = &m_light;
//	rPShader->SetParam("light").SetP(&m_light);
//}

void Grafkit::Light::Render(Grafkit::Renderer & deviceContext, Scene * scene)
{
	scene->GetPShader()->SetParamT<light_t>(deviceContext, "light", m_light);
}

// ============================================================================================================