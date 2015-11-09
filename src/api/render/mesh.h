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

	/**
	Simple mesh generator. Automatically creates the vertex buffers from vectors 
	*/
	class SimpleMeshGenerator {
		public: 
			SimpleMeshGenerator(ID3D11Device* device) : m_device(device) {}

		MeshRef& operator() (
			int vertexCount, const float3* position,
			int indexCount, const int* indices
		);

		MeshRef& operator() (
			int vertexCount, const float3* position, const float3* normal, const float2* uv, const float3* tangent,
			int indexCount, const int* indices
		);

	private:
		ID3D11Device* m_device;
	};
}

#endif
