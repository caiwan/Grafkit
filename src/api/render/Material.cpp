#include "Material.h"

#include "shader.h"
#include "texture.h"

#include "exceptions.h"

#include "easyloggingpp.h"

//#include "../core/renderassets.h"

using namespace  Grafkit;

using FWdebug::Exception;
using namespace FWdebugExceptions;

using std::vector;

// ====================================


//void Grafkit::MaterialBase::ReflectShader()
//{
//	// reflect cbuffer locations 
//	ReflectTextures();
//}
//
//void Grafkit::MaterialBase::ReflectTextures()
//{
//
//	if (this->m_framgentShader.Invalid() || this->m_framgentShader->Invalid()) {
//		throw EX_DETAILS(NullPointerException, "Nincs shader asset, vagy nincs shader beallitva - esetleg nem toltodot be rendesen");
//		//LOG(TRACE) << "No valid shader was set, abort texture reflection.";
//		//return;
//	}
//
//	Shader* shader = m_framgentShader;
//
//	char txname[256];
//
//	m_reflected_textures.clear();
//
//	for (size_t i = 0; i < TT_COUNT; i++) 
//	{
//		if (!this->m_texture_buckets[i].empty())
//		{
//			for (size_t j = 0; j < m_texture_buckets[i].size(); j++)
//			{
//				Shader::BoundResourceRecord *brecord = nullptr; &(shader->GetBResource(txname));
//				// search for names
//				if (j == 0) {
//					sprintf_s(txname, "texture_%s", map_names[i]);
//					brecord = &(shader->GetBResource(txname));
//				}
//
//				if (brecord == nullptr || !brecord->IsValid())
//				{
//					sprintf_s(txname, "texture_%s%d", map_names[i], j);
//					brecord = &(shader->GetBResource(txname));
//				}
//				
//				if (brecord->IsValid()) {
//					reflection_texture_entity entity;
//
//					// remeljuk nem akad ossze az elozoleg besetelt textura, ami nincs ebben benne
//					entity.brecord = brecord;
//					entity.texture = m_texture_buckets[i][j]->Get();
//					m_reflected_textures.push_back(entity);
//
//					LOG(TRACE) << "Reflected texture" << txname;
//				}
//				else {
//					LOG(TRACE) << "No reflection point found" << txname;
//				}
//			}
//		}
//	}
//}

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

void Grafkit::MaterialBase::Render(ID3D11DeviceContext * deviceContext, ShaderRef &shader)
{
	shader["material"].Set(&m_material);

	for (textureMap_it_t it = this->m_textures.begin(); it != this->m_textures.end(); it++) {
		shader->GetBResource(it->first.c_str()).SetTexture(it->second);
	}
}
