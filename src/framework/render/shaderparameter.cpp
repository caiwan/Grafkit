#include "stdafx.h"

#include "shaderparameter.h"

using namespace Grafkit;

namespace {
}

Grafkit::ShaderParameter::ShaderParameter() : m_refresh(true)
{
}

Grafkit::ShaderParameter::~ShaderParameter()
{
}

void Grafkit::ShaderParameter::Initialize(Renderer & render, ShaderResRef shader)
{
	m_targetShader = shader;
	m_lastShader = shader->Get();
	UpdateTargets();
}

void Grafkit::ShaderParameter::SetParam(std::string name, void * ptr)
{
	int32_t id = -1;
	if (m_lastShader) {
		id = m_lastShader->GetParamId(name);
	}
	if (id == -1)
		m_refresh |= true;

	Param_T param = { id, ptr };
	m_paramMap[name] = param;
}

void Grafkit::ShaderParameter::SetSampler(std::string name, TextureSamplerRef sampler)
{
	int32_t id = -1;
	if (m_lastShader) {
		id = m_lastShader->GetBoundedResourceId(name);
	}
	if (id == -1)
		m_refresh |= true;

	Sampler_T smaplerParam = { id, sampler };
	m_smaplerMap[name] = smaplerParam;
}

void Grafkit::ShaderParameter::SetATexture(std::string name, Ref<IResource> texture)
{
	int32_t id = -1;
	if (m_lastShader) {
		id = m_lastShader->GetBoundedResourceId(name);
	}

	if (id == -1)
		m_refresh |= true;

	Texture_T textureParam = { id, texture };
	m_textureMap[name] = textureParam;
}


void Grafkit::ShaderParameter::BindParameters(Renderer & render)
{
	if (m_targetShader->Get() != m_lastShader || m_refresh)
		UpdateTargets();

	for (auto it = m_paramMap.begin(); it != m_paramMap.end(); it++) {
		if (it->second.id != -1)
			m_lastShader->SetParam(render, it->second.id, it->second.p);
	}

	for (auto it = m_textureMap.begin(); it != m_textureMap.end(); it++) {
		if (it->second.id != -1) {
			ATexture *texture = dynamic_cast<ATexture*>(dynamic_cast<Resource<ATexture*>*>(it->second.texture.Get()));
			m_lastShader->SetShaderResourceView(render, it->second.id, *(texture));
		}
	}

	for (auto it = m_smaplerMap.begin(); it != m_smaplerMap.end(); it++) {
		if (it->second.id != -1)
			m_lastShader->SetSamplerSatate(render, it->second.id, *(it->second.sampler));
	}

}


void Grafkit::ShaderParameter::UpdateTargets()
{
	m_lastShader = m_targetShader->Get();
	for (auto it = m_paramMap.begin(); it != m_paramMap.end(); it++) {
		it->second.id = m_lastShader->GetParamId(it->first);
	}

	for (auto it = m_textureMap.begin(); it != m_textureMap.end(); it++) {
		it->second.id = m_lastShader->GetBoundedResourceId(it->first);
	}

	for (auto it = m_smaplerMap.begin(); it != m_smaplerMap.end(); it++) {
		it->second.id = m_lastShader->GetBoundedResourceId(it->first);
	}
}

