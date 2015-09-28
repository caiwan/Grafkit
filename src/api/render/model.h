/**
*/

#ifndef _Model_H_
#define _Model_H_

#include "renderer.h"
#include "dxtypes.h"

#include "../core/reference.h"

namespace FWrender {

	class Model : public Referencable
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

		bool Initialize(
			ID3D11Device* device, 
			FWrender::TextureRef texture,
			int indicesCount, const int* indices,
			int verticesCount, const float3* position, const float2* uv
		);
		
		inline void Shutdown();
		void Render(ID3D11DeviceContext* deviceContext);

		int GetIndexCount() { return m_indexCount; }

		ID3D11ShaderResourceView* GetTexture();

	private:
		void ShutdownBuffers();
		void RenderBuffers(ID3D11DeviceContext* deviceContext);

	private:
		FWrender::TextureRef m_texture;

		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
		int m_vertexCount, m_indexCount;
	};

}
#endif
