/**
*/

#ifndef _Mesh_H_
#define _Mesh_H_

#include <vector>
#include <map>

#include "renderer.h"
#include "dxtypes.h"

#include "reference.h"
#include "exceptions.h"

#include "shader.h"


namespace FWrender 
{
	/**
	Stores basic data for a piece of mesh
	*/
	class Mesh : public virtual Referencable{
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
			// void RenderBuffers(ID3D11DeviceContext* deviceContext);
			// void ShutdownBuffers();
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
			// std::vector<BufferStateDescriptor> m_buffers;
			BufferStateDescriptor m_buffer;
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

			void setPtr(std::string name, void* ptr) { this->m_mapPtr[name] = ptr; }

		// --- 
		public:
			// --- 
			/// param setter class
			class MeshAttribSetter {
				friend class SimpleMeshGenerator;
				protected:
					MeshAttribSetter(const char*& name, SimpleMeshGenerator& parent) : m_parent(parent), m_name(name) {}
				public:
					void operator =(void* ptr) { this->m_parent.setPtr(m_name, ptr); }

				private:
					SimpleMeshGenerator& m_parent;
					// const char *&m_name;
					std::string m_name;

					MeshAttribSetter(MeshAttribSetter& other) : m_parent(other.m_parent), m_name(other.m_name) {}
					void operator = (MeshAttribSetter&) {}
			};

			MeshAttribSetter operator[](const char*name) { return MeshAttribSetter(name, *this); }

			/**
			Layout assembler
			*/
			MeshRef operator() (size_t vertexCount, size_t indexCount, const int* indices, MeshRef input = NULL);

		private:
			void createIndexBuffer(MeshRef in_mesh, int indexCount, const int* indices);

		private:
			ID3D11Device *m_device;
			ShaderRef m_shader;

			typedef std::map<std::string, void*> mapPtr_t;
			mapPtr_t m_mapPtr;
	};
}

// --- excpetions 
DEFINE_EXCEPTION(CreateVertexBufferException, 1301, "Could not create vertex buffer")
DEFINE_EXCEPTION(CreateIndevBufferException, 1302, "Could not create index buffer")

#endif
