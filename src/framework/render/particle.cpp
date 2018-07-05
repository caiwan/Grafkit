#include "stdafx.h"

#include "particle.h"

#include "texture.h"

Grafkit::ParticleEngine::ParticleEngine()
{
	ZeroMemory(&m_shaderParams, sizeof(m_shaderParams));
}

Grafkit::ParticleEngine::~ParticleEngine()
{
}

void Grafkit::ParticleEngine::AddDynamics(ParticleDynamicsRef elem)
{
	m_dynamicElements.push_back(elem);
}

void Grafkit::ParticleEngine::Initialize(Renderer & render, ShaderResRef engine, size_t particleRes)
{
	m_fsParticleEngine = engine;

	m_particleCompute = new Compute();
	m_particleCompute->AddChannel("tex_age");
	m_particleCompute->AddChannel("tex_acceleration");
	m_particleCompute->AddChannel("tex_velocity");
	m_particleCompute->AddChannel("tex_position");
	m_particleCompute->AddChannel("tex_color");

	m_particleCompute->Initialize(render, m_fsParticleEngine, particleRes);
}

void Grafkit::ParticleEngine::Render(Renderer & render)
{
	size_t paramCount = m_dynamicElements.size();
	paramCount = (PARTICLE_NG_MAX_ELEM < paramCount) ? PARTICLE_NG_MAX_ELEM : paramCount;
	size_t activeParams = 0;
	for (size_t i = 0; i < paramCount; i++) {
		if (m_dynamicElements[i]->m_enabled) {
			m_dynamicElements[i]->Calculate();
			m_shaderParams.args[activeParams] = m_dynamicElements[i]->m_params;
			activeParams++;
		}
	}

	m_shaderParams.elemCount = activeParams;
	(*m_fsParticleEngine)->SetParamT(render, "ParticleEngineParams", m_shaderParams);
	m_particleCompute->Render(render);
}

// ========================================================================

Grafkit::ParticleDynamics::ParticleDynamics(float4 position, float weight) : m_enabled(true)
{
	ZeroMemory(&m_params, sizeof(m_params));
	m_params.args.position = position;
	m_params.weight = weight;
}

void Grafkit::ParticleDynamics::Calculate()
{
	m_params.type = GetType();
}

// ========================================================================

Grafkit::ParticleAttractor::ParticleAttractor(float4 pos, float weight, float force) : Grafkit::ParticleDynamics(pos, weight)
{
	m_params.args.param0.x = force;
}
