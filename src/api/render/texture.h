#pragma once

#include "dxtypes.h"
#include "reference.h"
#include "exceptions.h"

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

		ID3D11Texture2D* GetTexture2D() { return this->m_pTex; }
		ID3D11ShaderResourceView* GetTextureResource() { return this->m_pResourceView; }
		ID3D11SamplerState*	GetSamplerState() { return this->m_pSamplerState; }

		operator ID3D11Texture2D* () { return this->m_pTex; }
		operator ID3D11ShaderResourceView* () { return this->m_pResourceView; }

	private:
		ID3D11Texture2D * m_pTex;
		ID3D11ShaderResourceView* m_pResourceView;
		ID3D11SamplerState *m_pSamplerState;
	};
}

// load texture from the texture repository 
#define LOAD_TEXTURE(filename) 

DEFINE_EXCEPTION(BitmapLoadException, 1105, "Could not load and create bitmap from file")
DEFINE_EXCEPTION(TextureCreateException, 1105, "Could not create texture")
DEFINE_EXCEPTION(ShaderResourceViewException, 1105, "Could not create shader resource view")
DEFINE_EXCEPTION(SamplerStateCreateException, 1105, "Could not create sampler state")
