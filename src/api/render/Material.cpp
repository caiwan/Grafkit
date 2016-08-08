#include "../stdafx.h"

#include "Material.h"

#include "shader.h"
#include "texture.h"

using namespace  Grafkit;

using FWdebug::Exception;
using namespace FWdebugExceptions;

using std::vector;

// ====================================

TextureResRef Grafkit::BaseMaterial::GetTexture(std::string bindName)
{
	auto it = this->m_textures.find(bindName);
	if (it != m_textures.end()) {
		return it->second;
	}
	return TextureResRef();
}

void Grafkit::BaseMaterial::SetTexture(TextureResRef texture, std::string bindName)
{
	auto it = this->m_textures.find(bindName);
	if (it != m_textures.end()) {
		it->second = texture;
	}
	else {
		this->AddTexture(texture, bindName);
	}
}

void Grafkit::BaseMaterial::AddTexture(TextureResRef texture, std::string bindName)
{
	m_textures[bindName] = texture;
}

void Grafkit::BaseMaterial::RemoveTexture(TextureResRef texture, std::string bindName)
{
	auto it = this->m_textures.find(bindName);
	if (it != m_textures.end()) {
		m_textures.erase(it);
	}
}

// ====================================

void Grafkit::BaseMaterial::Render(Renderer& render, ShaderRef &_shader)
{
	ShaderRef shader = _shader; 

	//if (m_override_fshader.Valid()) {
	//	m_override_fshader->Render(render);
	//	shader = m_override_fshader;
	//}
	
	shader->GetParam("material").SetP(&m_material);

	for (auto it = this->m_textures.begin(); it != this->m_textures.end(); it++) {
		if (it->second.Valid() && it->second->Valid())
			shader->GetBRes(it->first).Set((*it->second)->GetTextureResource());
	}
}
