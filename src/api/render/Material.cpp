#include "Material.h"

#include "../utils/exceptions.h"
#include "../utils/logger.h"

#include "shader.h"
#include "texture.h"

using namespace  Grafkit;

using FWdebug::Exception;
using namespace FWdebugExceptions;

using std::vector;

// ====================================

TextureResRef Grafkit::MaterialBase::GetTexture(std::string bindName)
{
	textureMap_it_t it = this->m_textures.find(bindName);
	if (it != m_textures.end()) {
		return it->second;
	}
	return TextureResRef();
}

void Grafkit::MaterialBase::SetTexture(TextureResRef texture, std::string bindName)
{
	textureMap_it_t it = this->m_textures.find(bindName);
	if (it != m_textures.end()) {
		it->second = texture;
	}
	else {
		this->AddTexture(texture, bindName);
	}
}

void Grafkit::MaterialBase::AddTexture(TextureResRef texture, std::string bindName)
{
	m_textures[bindName] = texture;
}

void Grafkit::MaterialBase::RemoveTexture(TextureResRef texture, std::string bindName)
{
	textureMap_it_t it = this->m_textures.find(bindName);
	if (it != m_textures.end()) {
		m_textures.erase(it);
	}
}

// ====================================

void Grafkit::MaterialBase::Render(Renderer& render, ShaderRef &shader)
{
	(*shader)["material"] = &m_material;

	for (textureMap_it_t it = this->m_textures.begin(); it != this->m_textures.end(); it++) {
		shader->GetBRes(it->first) = (ID3D11ShaderResourceView*)(**it->second);
	}
}
