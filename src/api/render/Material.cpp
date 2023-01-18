#include "Material.h"

#include "shader.h"
#include "texture.h"

#include "easyloggingpp.h"

using FWrender::Renderer;
using FWrender::Texture;
using FWrender::Shader;
using FWrender::TextureAssetRef;
using FWrender::TextureRef;
using FWrender::ShaderRef;

using std::vector;

FWrender::MaterialBase::MaterialBase()
{
	for (size_t i = 0; i < TT_COUNT; i++) {
		this->m_texture_buckets[i] = vector<TextureAssetRef>();
	}
}

FWrender::MaterialBase::~MaterialBase()
{
	for (size_t i = 0; i < TT_COUNT; i++) {
		this->m_texture_buckets[i].clear();
	}
}

/** Texture names LUT*/
namespace {
	const char *map_names [] = 
	{
		"diffuse",
		"alpha",
		"normal",
		"shiniess",
		"specular",
		"selfillum",
		"reflect",
		"bump",
		"aux",
	};
}

void FWrender::MaterialBase::Render(ID3D11DeviceContext * deviceContext)
{
	// setup cbuffers

	// setup texutes 
	for (size_t i = 0; i < this->m_reflected_textures.size(); i++) {
		this->m_reflected_textures[i].brecord->SetTexture(this->m_reflected_textures[i].texture);
	}
}

// ====================================

TextureAssetRef & FWrender::MaterialBase::GetTexture(texture_type_e bucket, int n)
{
	/// @todo bounds check
	return m_texture_buckets[bucket][n];
}

void FWrender::MaterialBase::SetTexture(TextureAssetRef texture, texture_type_e bucket, int n)
{
	/// @todo bounds check
	m_texture_buckets[bucket][n] = texture;
}

void FWrender::MaterialBase::AddTexture(TextureAssetRef texture, texture_type_e bucket)
{
	m_texture_buckets[bucket].push_back(texture);
}

// ====================================

void FWrender::MaterialBase::ReflectShader()
{
	// reflect cbuffer locations 
	ReflectTextures();
}

void FWrender::MaterialBase::ReflectTextures()
{
	if (this->m_framgentShader.Invalid()) {
		LOG(TRACE) << "No valid shader was set, abort texture reflection.";
		return;
	}

	char txname[256];

	m_reflected_textures.clear();

	for (size_t i = 0; i < TT_COUNT; i++) 
	{
		if (!this->m_texture_buckets[i].empty())
		{
			for (size_t j = 0; j < m_texture_buckets[i].size(); j++)
			{
				Shader::BoundResourceRecord *brecord = nullptr; &(this->m_framgentShader->GetBResource(txname));
				// search for names
				if (j == 0) {
					sprintf_s(txname, "texture_%s", map_names[i]);
					brecord = &(this->m_framgentShader->GetBResource(txname));
				}

				if (brecord == nullptr || !brecord->IsValid())
				{
					sprintf_s(txname, "texture_%s%d", map_names[i], j);
					brecord = &(this->m_framgentShader->GetBResource(txname));
				}
				
				if (brecord->IsValid()) {
					struct reflection_texture_entity entity;

					// remeljuk nem akad ossze az elozoleg besetelt textura, ami nincs ebben benne
					entity.brecord = brecord;
					entity.texture = m_texture_buckets[i][j];
					m_reflected_textures.push_back(entity);

					LOG(TRACE) << "Reflected texture" << txname;
				}
				else {
					LOG(TRACE) << "No reflection point found" << txname;
				}
			}
		}
	}
}