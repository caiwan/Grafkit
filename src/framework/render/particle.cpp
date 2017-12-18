#include "stdafx.h"

#include "particle.h"


Grafkit::ParticleEngine::ParticleEngine()
{
}

Grafkit::ParticleEngine::~ParticleEngine()
{
}

void Grafkit::ParticleEngine::AddDynamicElem(ParticleDynamicsRef & elem)
{
	m_dynamicElements.push_back(elem);
}

void Grafkit::ParticleEngine::Initialize(Renderer & render, ShaderResRef engine, size_t particleRes)
{
	m_fsParticleEngine = engine;

	m_particleCompute = new Compute();
	m_particleCompute->AddChannel("tex_age");
	m_particleCompute->AddChannel("tex_velocity");
	m_particleCompute->AddChannel("tex_speed");
	m_particleCompute->AddChannel("tex_position");

	m_particleCompute->Initialize(render, m_fsParticleEngine, particleRes);

	ZeroMemory(&m_shaderParams, sizeof(engineParams_t));
}

void Grafkit::ParticleEngine::Render(Renderer & render)
{
	size_t paramCount = m_dynamicElements.size();
	paramCount = (PARTICLE_NG_MAX_ELEM < paramCount) ? PARTICLE_NG_MAX_ELEM : paramCount;
	for (size_t i = 0; i < paramCount; i++) {
		m_dynamicElements[i]->Calculate();
		m_shaderParams.params[i] = m_dynamicElements[i]->dynamicParams;
	}

	//(*m_fsParticleEngine);

	m_particleCompute->Render(render);
}
