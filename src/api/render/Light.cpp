#include "stdafx.h"

#include "Light.h"

#include "Scene.h"

using namespace Grafkit;

Light::Light() : Entity3D()
{
	m_position = float4(0, 0, 0, 1);
	m_direction = float4(0, 0, -1, 0);
	ZeroMemory(&m_light, sizeof(m_light));
	m_light.la = 1.0;
}

Light::~Light()
{
}

void Grafkit::Light::Calculate(Grafkit::Renderer & deviceContext, Scene * const & scene, Matrix &nodeMatrix)
{
	m_position.w = 1.;
	m_direction.w = 0.;
	m_light.position = nodeMatrix.Transfrom(m_position);
	m_light.direction = nodeMatrix.Transfrom(m_direction);
}

void Grafkit::Light::Render(Grafkit::Renderer & deviceContext, Scene * const & scene)
{
	scene->GetPShader()->SetParamT<light_t>(deviceContext, "light", m_light);
}

// ============================================================================================================