#include "stdafx.h"

#include "Material.h"

#include "shader.h"
#include "texture.h"

using namespace  Grafkit;

using FWdebug::Exception;
using namespace FWdebugExceptions;

Material* Material::g_lastMaterial = nullptr;

PERSISTENT_IMPL(Grafkit::Material);

namespace {
    const char *texture_map_names[Material::TT_COUNT] =
    {
        "t_diffuse",
        "t_metallic",
        "t_roughness",
        "t_emission",
        "t_alpha",
        "t_normal",
        "t_bump",
        "t_aux0",
        "t_aux1",
        "t_aux1",
        "t_aux3"
    };
}

// ====================================

Material::Material() : Object()
, m_lastVShader(nullptr)
, m_lastPShader(nullptr)
, m_colors_id(0)
, m_params_id(0)
{
    CreateUuid();

    ZeroMemory(&m_colors, sizeof(m_colors));
    ZeroMemory(&m_params, sizeof(m_params));

    m_colors.diffuse = float4(1, 1, 1, 1);
}

TextureResRef Material::GetTexture(std::string bindName) const
{
    auto it = this->m_textures.find(bindName);
    if (it != m_textures.end()) {
        return it->second;
    }
    return TextureResRef();
}

void Material::SetTexture(TextureResRef texture, std::string bindName)
{
    auto it = this->m_textures.find(bindName);
    if (it != m_textures.end()) {
        it->second = texture;
    }
    else {
        this->AddTexture(texture, bindName);
    }
}

void Material::AddTexture(TextureResRef texture, std::string bindName)
{
    m_textures[bindName] = texture;
}

void Material::RemoveTexture(TextureResRef texture, std::string bindName)
{
    auto it = this->m_textures.find(bindName);
    if (it != m_textures.end()) {
        m_textures.erase(it);
    }
}

void Material::SetTexture(TextureResRef texture, texture_type_e slot)
{
    this->SetTexture(texture, texture_map_names[slot]);
}

void Material::AddTexture(TextureResRef texture, texture_type_e slot)
{
    this->AddTexture(texture, texture_map_names[slot]);
}

void Material::RemoveTexture(TextureResRef texture, texture_type_e slot)
{
    this->RemoveTexture(texture, texture_map_names[slot]);
}

// ====================================

void Material::Render(Renderer& render, ShaderRef &vs, ShaderRef &fs)
{
#if 0
    if (g_lastMaterial == this && m_lastVShader == vs && m_lastPShader == fs)
        return;

    g_lastMaterial = this;
    if (m_lastVShader != fs || m_lastPShader) {
        m_lastPShader = fs;
        m_lastVShader = vs;
        //m_colors_id = vs->GetParamId(render, )
        //m_params_id = 
    }
#endif 

    vs->SetParam(render, "material_colors", &m_colors);
    fs->SetParam(render, "material_params", &m_params);

    for (int i = 0; i < TT_COUNT; i++) {
        fs->SetBoundedResourcePointer(render, texture_map_names[i], nullptr);
    }

    for (auto it = this->m_textures.begin(); it != this->m_textures.end(); ++it) {
        if (it->second.Valid() && it->second->Valid()) {
            vs->SetShaderResourceView(render, it->first, static_cast<ID3D11ShaderResourceView*>(**it->second));
            fs->SetShaderResourceView(render, it->first, static_cast<ID3D11ShaderResourceView*>(**it->second));
        }
        else {
            vs->SetShaderResourceView(render, it->first, nullptr);
            fs->SetShaderResourceView(render, it->first, nullptr);
        }
    }
}

// ====================================

bool Material::GetTextureMap(std::map<std::string, TextureResRef>& textureMap) const
{
    textureMap.clear();

    if (m_textures.empty())
        return false;

    textureMap.insert(m_textures.cbegin(), m_textures.cend());

    return true;
}

void Material::Serialize(Archive & ar)
{
    _Serialize(ar);

    PERSIST_FIELD(ar, m_colors);
    PERSIST_FIELD(ar, m_params);
}
