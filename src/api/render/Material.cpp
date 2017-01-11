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
		"t_alpha",
		"t_normal",
		"t_shiniess",
		"t_specular",
		"t_selfillum",
		"t_reflect",
		"t_bump",
		"t_aux0",
		"t_aux1",
		"t_aux1",
		"t_aux3"
	};
}

// ====================================

Grafkit::Material::Material(enum Material::material_type_e t) : IResource() , Persistent()
{
	ZeroMemory(&m_material, sizeof(m_material));
	m_material.diffuse = float4(1, 1, 1, 0);
	
	m_material.type = t;
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
	this->m_material.has_texture[slot] = 1;
}

void Grafkit::Material::AddTexture(TextureResRef texture, texture_type_e slot)
{
	this->AddTexture(texture, texture_map_names[slot]);
	this->m_material.has_texture[slot] = 1;
}

void Grafkit::Material::RemoveTexture(TextureResRef texture, texture_type_e slot)
{
	this->RemoveTexture(texture, texture_map_names[slot]);
	this->m_material.has_texture[slot] = 0;
}

// ====================================

void Grafkit::Material::Render(Renderer& render, ShaderRef &_shader)
{
	ShaderRef shader = _shader; 

	shader->SetParam(render, "material", &m_material);

	for (auto it = this->m_textures.begin(); it != this->m_textures.end(); it++) {
		if (it->second.Valid() && it->second->Valid())
			shader->SetBoundedResourcePointer(it->first, (*it->second)->GetTextureResource());
	}
}

// ====================================
void Grafkit::Material::serialize(Archive & ar)
{
	this->IResource::_serialize(ar);

	// ...
	PERSIST_FIELD(m_material);
	
	/// @todo override shader + 
	/// @todo textures 
}