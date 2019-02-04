#include "stdafx.h"

#include "Light.h"

#include "SceneGraph.h"

PERSISTENT_IMPL(Grafkit::Light);

using namespace Grafkit;

Light::Light() : Entity3D()
    , m_type(LT_point)
    , m_id(0)
{
    m_position = float4(0, 0, 0, 1);
    //m_direction = float4(0, 0, -1, 0);
    ZeroMemory(&m_light, sizeof(m_light));
    m_light.la = 1.0;
    m_light.intensity = 1.;
    m_light.color = float4(1, 1, 1, 1);
}

Light::Light(light_type_t t) : Entity3D()
    , m_type(t)
    , m_id(0)
{
    m_position = float4(0, 0, 0, 1);
    //m_direction = float4(0, 0, -1, 0);
    ZeroMemory(&m_light, sizeof(m_light));
    m_light.la = 1.0;
    m_light.intensity = 1.;
    m_light.color = float4(1, 1, 1, 1);
}

Light::~Light() {
}

void Light::Calculate(Renderer& deviceContext, const ActorRef & parent)
{
    assert(parent);

    Matrix nodeMatrix = parent->WorldMatrix();

    m_position.w = 1.;
    m_light.position = nodeMatrix.Transfrom(m_position);
}

size_t Light::GetInternalData(void* const & p)
{
    CopyMemory(p, &m_light, sizeof(light2_t));
    return sizeof(light2_t);
}

void Light::Render(Renderer& deviceContext, SceneGraph& scene) {
}

