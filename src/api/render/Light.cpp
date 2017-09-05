#include "stdafx.h"

#include "Light.h"

#include "Scene.h"

using namespace Grafkit;

#define PERSISTENT_IMPL(Light);

Grafkit::Light::Light(light_type_t t) : Entity3D(), m_type(t), m_id(0)
{
	m_position = float4(0, 0, 0, 1);
	//m_direction = float4(0, 0, -1, 0);
	ZeroMemory(&m_light, sizeof(m_light));
	m_light.la = 1.0;
	m_light.color = float4(1, 1, 1, 1);
}

Light::~Light()
{
}

void Grafkit::Light::Calculate(Grafkit::Renderer & deviceContext, Scene * const & scene, Matrix &nodeMatrix)
{
	m_position.w = 1.;
	//m_direction.w = 0.;
	m_light.position = nodeMatrix.Transfrom(m_position);
	//m_light.direction = nodeMatrix.Transfrom(m_direction);
}

size_t Grafkit::Light::GetInternalData(void * const & p)
{
	CopyMemory(p, &m_light, sizeof(light2_t*));
	return sizeof(light2_t*);
}

void Grafkit::Light::serialize(Archive & ar)
{
	Entity3D::_serialize(ar);

	PERSIST_FIELD(ar, m_position);
	//PERSIST_FIELD(ar, m_direction);
	PERSIST_FIELD(ar, m_light);
}

void Grafkit::Light::Render(Grafkit::Renderer & deviceContext, Scene * const & scene)
{
	//scene->GetPShader()->SetParamT<light_t>(deviceContext, "light", m_light);
	// we should have something else here
}

// ============================================================================================================