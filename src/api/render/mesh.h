/**
*/

#ifndef _Model_H_
#define _Model_H_

#include "renderer.h"
#include "dxtypes.h"

#include "../core/reference.h"

#include <vector>

namespace FWrender 
{
	/**
	Stores basic data for a piece of mesh
	*/
	class Mesh : public virtual Referencable{
		public:
			struct VertexType
			{
				float3 position;
				float2 texture;
			};

		public:
			Mesh();
			Mesh(const Mesh& mesh);
			virtual ~Mesh();

			void Render(ID3D11DeviceContext* deviceContext);
			void Shutdown();

			///@todo ez legyen kulon valasztva mindentol
			// void CreateVertexBuffer(ID3D11Device* device, int vertexCount, const float3* position, const float2* uv);
			// void CreateIndexBuffer(ID3D11Device* device, int indexCount, const int* indices);

			void addElement(ID3D11Buffer * pBuffer) { this->m_buffers.push_back(pBuffer); }
			void addIndices(ID3D11Buffer * pBuffer) { this->m_buffers.push_back(pBuffer); }

			int GetIndexCount() { return m_indexCount; }

		protected:
			void RenderBuffers(ID3D11DeviceContext* deviceContext);
			void ShutdownBuffers();
		

		private:
			// ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
			ID3D11Buffer *m_indexBuffer;
			std::vector<ID3D11Buffer *> m_buffers;
			int m_vertexCount, m_indexCount;
	};

	typedef Ref<Mesh> MeshRef;

	class SimpleMeshGenerator {
	public: 
		static MeshRef fromRaw(
			ID3D11Device* device,
			int vertexCount, const float3* position, const float2* uv,
			int indexCount, const int* indices
		);

	};
}

#endif
