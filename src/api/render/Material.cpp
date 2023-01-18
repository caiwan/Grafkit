#include "Material.h"

#include "shader.h"
#include "texture.h"

#include "exceptions.h"

#include "easyloggingpp.h"

//#include "../core/renderassets.h"

using Grafkit::Renderer;
using Grafkit::Texture;
using Grafkit::Shader;
using Grafkit::TextureAssetRef;
using Grafkit::TextureRef;
using Grafkit::ShaderRef;

using FWdebug::Exception;
using namespace FWdebugExceptions;

using std::vector;

Grafkit::MaterialBase::MaterialBase()
{
	for (size_t i = 0; i < TT_COUNT; i++) {
		this->m_texture_buckets[i] = vector<TextureAssetRef>();
	}
}

Grafkit::MaterialBase::~MaterialBase()
{
	for (size_t i = 0; i < TT_COUNT; i++) {
		this->m_texture_buckets[i].clear();
	}
}

/** Texture names LUT*/
namespace {
	const char *map_names [Grafkit::TT_COUNT] =
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

void Grafkit::MaterialBase::Render(ID3D11DeviceContext * deviceContext)
{
	// setup cbuffers

	// setup texutes 
	for (size_t i = 0; i < this->m_reflected_textures.size(); i++) {
		this->m_reflected_textures[i].brecord->SetTexture(this->m_reflected_textures[i].texture);
	}
}

// ====================================

TextureAssetRef & Grafkit::MaterialBase::GetTexture(texture_type_e bucket, int n)
{
	/// @todo bounds check
	return m_texture_buckets[bucket][n];
}

void Grafkit::MaterialBase::SetTexture(TextureAssetRef texture, texture_type_e bucket, int n)
{
	/// @todo bounds check
	m_texture_buckets[bucket][n] = texture;
}

void Grafkit::MaterialBase::AddTexture(TextureAssetRef texture, texture_type_e bucket)
{
	m_texture_buckets[bucket].push_back(texture);
}

// ====================================

void Grafkit::MaterialBase::ReflectShader()
{
	// reflect cbuffer locations 
	ReflectTextures();
}

void Grafkit::MaterialBase::ReflectTextures()
{

	if (this->m_framgentShader.Invalid() || this->m_framgentShader->Invalid()) {
		throw EX_DETAILS(NullPointerException, "Nincs shader asset, vagy nincs shader beallitva - esetleg nem toltodot be rendesen");
		//LOG(TRACE) << "No valid shader was set, abort texture reflection.";
		//return;
	}

	Shader* shader = m_framgentShader;

	char txname[256];

	m_reflected_textures.clear();

	for (size_t i = 0; i < TT_COUNT; i++) 
	{
		if (!this->m_texture_buckets[i].empty())
		{
			for (size_t j = 0; j < m_texture_buckets[i].size(); j++)
			{
				Shader::BoundResourceRecord *brecord = nullptr; &(shader->GetBResource(txname));
				// search for names
				if (j == 0) {
					sprintf_s(txname, "texture_%s", map_names[i]);
					brecord = &(shader->GetBResource(txname));
				}

				if (brecord == nullptr || !brecord->IsValid())
				{
					sprintf_s(txname, "texture_%s%d", map_names[i], j);
					brecord = &(shader->GetBResource(txname));
				}
				
				if (brecord->IsValid()) {
					reflection_texture_entity entity;

					// remeljuk nem akad ossze az elozoleg besetelt textura, ami nincs ebben benne
					entity.brecord = brecord;
					entity.texture = m_texture_buckets[i][j]->Get();
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