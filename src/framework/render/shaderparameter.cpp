#include "stdafx.h"

#include "core/resource.h"
#include "shaderparameter.h"
#include "Texture.h"

using namespace Grafkit;

namespace {
}

ShaderParameter::ShaderParameter() : m_refresh(true)
{
}

ShaderParameter::~ShaderParameter()
{
}

void ShaderParameter::Initialize(Renderer & render, ShaderResRef shader)
{
    m_targetShader = shader;
    m_lastShader = shader->Get();
    UpdateTargets();
}

void ShaderParameter::SetParam(std::string name, void * ptr)
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

void ShaderParameter::SetSampler(std::string name, TextureSamplerRef sampler)
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



void ShaderParameter::BindParameters(Renderer & render)
{
    if (m_targetShader->Get() != m_lastShader || m_refresh)
        UpdateTargets();

    for (auto it = m_paramMap.begin(); it != m_paramMap.end(); ++it) {
        if (it->second.id != -1)
            m_lastShader->SetParam(render, it->second.id, it->second.p);
    }

    for (auto it = m_textureMap.begin(); it != m_textureMap.end(); ++it) {
        if (it->second.id != -1) {
            Resource<ATexture>* texture = dynamic_cast<Resource<ATexture>*>(it->second.texture.Get());
            m_lastShader->SetShaderResourceView(render, it->second.id, static_cast<ID3D11ShaderResourceView*>(**texture));
        }
    }

    for (auto it = m_smaplerMap.begin(); it != m_smaplerMap.end(); ++it) {
        if (it->second.id != -1)
            m_lastShader->SetSamplerSatate(render, it->second.id, *(it->second.sampler));
    }

}


void ShaderParameter::SetATexture(std::string name, Ref<IResource> texture)
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


void ShaderParameter::UpdateTargets()
{
    m_lastShader = m_targetShader->Get();
    for (auto it = m_paramMap.begin(); it != m_paramMap.end(); ++it) {
        it->second.id = m_lastShader->GetParamId(it->first);
    }

    for (auto it = m_textureMap.begin(); it != m_textureMap.end(); ++it) {
        it->second.id = m_lastShader->GetBoundedResourceId(it->first);
    }

    for (auto it = m_smaplerMap.begin(); it != m_smaplerMap.end(); ++it) {
        it->second.id = m_lastShader->GetBoundedResourceId(it->first);
    }
}

