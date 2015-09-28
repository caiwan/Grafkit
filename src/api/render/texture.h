#pragma once

#include "dxtypes.h"
#include "../core/reference.h"

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
		ID3D11ShaderResourceView* GetTexture() { return this->m_texture_resource; }

	private:
		ID3D11Texture2D *m_tex;
		ID3D11ShaderResourceView* m_texture_resource;
	};
}

// load texture from the texture repository 
#define LOAD_TEXTURE(filename) 