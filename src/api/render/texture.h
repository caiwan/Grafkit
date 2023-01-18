#pragma once

#include "dxtypes.h"
#include "../core/reference.h"
#include "../core/exceptions.h"

#define MULTITEXTURE_MAX 16

namespace FWrender
{
	class Texture : public Referencable
	{
	public:
		Texture();
		Texture(const Texture&);
		~Texture();

		bool Initialize(ID3D11Device* device, WCHAR* filename);
		void Shutdown();

		ID3D11Texture2D* GetTexture2D() { return this->m_tex; }
		ID3D11ShaderResourceView* GetTextureResource() { return this->m_texture_resource; }
		ID3D11SamplerState*	GetSamplerState() { return this->m_sampleState; }

		operator ID3D11Texture2D* () { return this->m_tex; }
		operator ID3D11ShaderResourceView* () { return this->m_texture_resource; }

	private:
		ID3D11Texture2D *m_tex;
		ID3D11ShaderResourceView* m_texture_resource;
		ID3D11SamplerState *m_sampleState;
	};
}

// load texture from the texture repository 
#define LOAD_TEXTURE(filename) 

DEFINE_EXCEPTION(BitmapLoadException, 1105, "Could not load and create bitmap from file")
DEFINE_EXCEPTION(TextureCreateException, 1105, "Could not create texture")
DEFINE_EXCEPTION(ShaderResourceViewException, 1105, "Could not create shader resource view")
DEFINE_EXCEPTION(SamplerStateCreateException, 1105, "Could not create sampler state")