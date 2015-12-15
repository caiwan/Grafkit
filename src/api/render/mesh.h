/**
*/

#ifndef _Mesh_H_
#define _Mesh_H_

#include <vector>
#include <map>

#include "renderer.h"
#include "dxtypes.h"

#include "../core/reference.h"
#include "../core/exceptions.h"

#include "shader.h"


namespace FWrender 
{
	/**
	Stores basic data for a piece of mesh
	*/
	class Mesh : public virtual Referencable{
		public:
			/*
			struct VertexType_pos
			{
				float3 position;
			};

			// paddolni kell-e? 
			struct VertexType_pos_tex
			{
				float3 position;
				float2 texture;
				float3 normal;
				float3 tangent;
			};
			*/
		public:
			Mesh();
			Mesh(const Mesh& mesh);
			virtual ~Mesh();

			void Render(ID3D11DeviceContext* deviceContext);
			
			/// @todo erre jobb megoldast kell talalni
			void setVertexCount(int count) { this->m_vertexCount = count; }
			
			void addElement(ID3D11Buffer * pBuffer, UINT stride, UINT offset = 0);
			void addIndices(ID3D11Buffer * pBuffer, UINT count) { this->m_indexBuffer = pBuffer; this->m_indexCount = count;}

			int GetIndexCount() { return m_indexCount; }

		protected:
			void RenderBuffers(ID3D11DeviceContext* deviceContext);
			void ShutdownBuffers();
			void Shutdown();

		protected:
			struct BufferStateDescriptor {
				ID3D11Buffer *buffer;
				UINT stride, offset;

				BufferStateDescriptor() {
					buffer = NULL;
					stride, offset = 0;
				}
			};

		private:
			ID3D11Buffer *m_indexBuffer;
			std::vector<BufferStateDescriptor> m_buffers;
			int m_vertexCount, m_indexCount;
	};

	typedef Ref<Mesh> MeshRef;

	/**
	Simple mesh generator. Automatically creates the vertex buffers from vectors 
	*/


	class SimpleMeshGenerator {
		friend class MeshAttribSetter;
	public:
		SimpleMeshGenerator(ID3D11Device* device, ShaderRef shader);

		void setPtr(const char* name, void* ptr) { this->m_mapPtr[name] = ptr; }

	public:
		/// param setter class
		class MeshAttribSetter {
		protected:
			MeshAttribSetter(const char*& name, SimpleMeshGenerator& parent) : m_parent(parent), m_name(name) {}
		public:
			void operator =(void* ptr) { this->m_parent.setPtr(m_name, ptr); }

		private:
			SimpleMeshGenerator& m_parent;
			const char *&m_name;

			MeshAttribSetter(MeshAttribSetter& other) : m_parent(other.m_parent), m_name(other.m_name) {}
			void operator = (MeshAttribSetter&) {}
		};

		MeshAttribSetter operator[](const char*) {}

		void operator() (size_t vertexCount, size_t indexCount, const int* indices);

	private:
		ID3D11Device *m_device;
		ShaderRef m_shader;

		typedef std::map<const char*, void*, cmp_str> mapPtr_t;
		mapPtr_t m_mapPtr;
	};

# if 0		
		///@ todo gen from shader layout 


		// + operator[]	-ral be lehessen allitani a pointereket
		// + a operator() -ral csak index + shader input kell
		// a shaderbol kiszedi az input layout reflectort + ez alapjan osszelinkeli 
		// es megcsinalja a blobot

		MeshRef operator() (
			int vertexCount, const float3* position,
			int indexCount, const int* indices,
			MeshRef input = NULL
		);

		MeshRef operator() (
			int vertexCount, const float3* position, const float3* normal, const float2* uv, const float3* tangent,
			int indexCount, const int* indices,
			MeshRef input = NULL
		);

	private:
		void createIndexBuffer(MeshRef mesh, int indexCount, const int* indices);
		

	};

#endif 
}

// --- excpetions 
DEFINE_EXCEPTION(CreateVertexBufferException, 1301, "Could not create vertex buffer")
DEFINE_EXCEPTION(CreateIndevBufferException, 1302, "Could not create index buffer")

#endif
