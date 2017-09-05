#include "stdafx.h"

#include "Material.h"

#include "shader.h"
#include "texture.h"

using namespace  Grafkit;

using FWdebug::Exception;
using namespace FWdebugExceptions;

using std::vector;

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

Grafkit::Material::Material() : IResource() , Persistent()
{
	ZeroMemory(&m_colors, sizeof(m_colors));
	ZeroMemory(&m_params, sizeof(m_params));

	m_colors.diffuse = float4(1, 1, 1, 1);
}

TextureResRef Grafkit::Material::GetTexture(std::string bindName)
{
	auto it = this->m_textures.find(bindName);
	if (it != m_textures.end()) {
		return it->second;
	}
	return TextureResRef();
}

void Grafkit::Material::SetTexture(TextureResRef texture, std::string bindName)
{
	auto it = this->m_textures.find(bindName);
	if (it != m_textures.end()) {
		it->second = texture;
	}
	else {
		this->AddTexture(texture, bindName);
	}
}

void Grafkit::Material::AddTexture(TextureResRef texture, std::string bindName)
{
	m_textures[bindName] = texture;
}

void Grafkit::Material::RemoveTexture(TextureResRef texture, std::string bindName)
{
	auto it = this->m_textures.find(bindName);
	if (it != m_textures.end()) {
		m_textures.erase(it);
	}
}

void Grafkit::Material::SetTexture(TextureResRef texture, texture_type_e slot)
{
	this->SetTexture(texture, texture_map_names[slot]);
}

void Grafkit::Material::AddTexture(TextureResRef texture, texture_type_e slot)
{
	this->AddTexture(texture, texture_map_names[slot]);
}

void Grafkit::Material::RemoveTexture(TextureResRef texture, texture_type_e slot)
{
	this->RemoveTexture(texture, texture_map_names[slot]);
}

// ====================================

void Grafkit::Material::Render(Renderer& render, ShaderRef &vs, ShaderRef &fs)
{
	vs->SetParam(render, "material_colors", &m_colors);
	fs->SetParam(render, "material_params", &m_params);

	for (int i = 0; i < TT_COUNT; i++) {
		fs->SetBoundedResourcePointer(render, texture_map_names[i], nullptr);
	}

	for (auto it = this->m_textures.begin(); it != this->m_textures.end(); it++) {
		if (it->second.Valid() && it->second->Valid())
			fs->SetBoundedResourcePointer(render, it->first, (*it->second)->GetShaderResourceView());
		else
			fs->SetBoundedResourcePointer(render, it->first, nullptr);
	}
}

// ====================================
void Grafkit::Material::serialize(Archive & ar)
{
	this->IResource::_serialize(ar);
	
	PERSIST_FIELD(ar, m_colors);
	PERSIST_FIELD(ar, m_params);
}