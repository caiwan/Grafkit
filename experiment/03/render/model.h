/**
*/

#ifndef _Model_H_
#define _Model_H_

#include "texture.h"
#include "dxtypes.h"

namespace FWrender {

	class Model
	{
	private:
		struct VertexType
		{
			float3 position;
			float2 texture;
		};

	public:
		Model();
		Model(const Model&);
		~Model();

		bool Initialize(ID3D11Device*, WCHAR* textureFilename);
		void Shutdown();
		void Render(ID3D11DeviceContext*);

		int GetIndexCount();

		bool LoadTexture(ID3D11Device*, WCHAR*);
		void ReleaseTexture();

		ID3D11ShaderResourceView* GetTexture() { return m_texture ? m_texture->GetTexture() : 0; }

	private:
		bool InitializeBuffers(ID3D11Device*);
		void ShutdownBuffers();
		void RenderBuffers(ID3D11DeviceContext*);

	private:
		FWrender::Texture* m_texture;

		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
		int m_vertexCount, m_indexCount;
	};

}
#endif