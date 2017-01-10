#include "stdafx.h"

#include "Light.h"

using namespace Grafkit;

Light::Light() : Entity3D()
{
	ZeroMemory(&m_light, sizeof(m_light));
	m_light.la = 1.0;

	ZeroMemory(&m_position, sizeof(m_position));
	ZeroMemory(&m_direction, sizeof(m_direction));
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
	//scene->GetPShader()->
	// TODO 
}

// ============================================================================================================