#pragma once

#include "dxtypes.h"

namespace FWrender
{
	class Texture
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