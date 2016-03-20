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


namespace Grafkit 
{
	/**
	Stores basic data for a piece of mesh
	*/
	class Mesh : public virtual Referencable
	{
		public:
			Mesh();
			Mesh(const Mesh& mesh);
			virtual ~Mesh();

			void RenderMesh(ID3D11DeviceContext* deviceContext);
			
			/// @todo erre jobb megoldast kell talalni
			void setVertexCount(int count) { this->m_vertexCount = count; }
			
			void addElement(ID3D11Buffer * pBuffer, UINT stride, UINT offset = 0);
			void addIndices(ID3D11Buffer * pBuffer, UINT count) { this->m_indexBuffer = pBuffer; this->m_indexCount = count;}

			int GetIndexCount() { return m_indexCount; }

		protected:
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


	///@todo legyen kepes ne csak meglevo shader input layoutjabol letreehozni dolgokat
	/**
	Simple mesh generator. Automatically creates the vertex buffers from vectors 
	*/
	class SimpleMeshGenerator 
	{
		friend class MeshAttribSetter;
		public:
			SimpleMeshGenerator(ID3D11Device *const& device, ShaderRef &shader);

			void SetPtr(std::string name, const void* const ptr) { this->m_mapPtr[name] = ptr; }

		// --- 
		public:
			// --- 
			/// @todo ezt elrendezni kicsit
			/// param setter class
			class MeshAttribSetter {
				friend class SimpleMeshGenerator;
				protected:
					MeshAttribSetter(const char *& name, SimpleMeshGenerator& parent) : m_parent(parent), m_name(name) {}
				public:
					void operator =(const void* const ptr) { this->m_parent.SetPtr(m_name, ptr); }

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
			MeshRef operator() (size_t vertexCount, size_t indexCount, const int* const indices, MeshRef input = nullptr);

		private:
			void createIndexBuffer(MeshRef in_mesh, int indexCount, const int* indices);

		private:
			ID3D11Device * const & m_device;
			ShaderRef &m_shader;

			typedef std::map<std::string, const void* > mapPtr_t;
			mapPtr_t m_mapPtr;
	};

	// ================================================================================================================================================
	///@tod ezt a jovoben kicsit szebben ki kellene vitelezni
	class QuadGenerator {
	public:
		QuadGenerator(ID3D11Device *const& device, ShaderRef &shader) : m_meshGen(device, shader) {}
		~QuadGenerator() {}

		MeshRef operator() (MeshRef input = nullptr);

	private:
		SimpleMeshGenerator m_meshGen;
	};
}

// --- excpetions 
DEFINE_EXCEPTION(CreateVertexBufferException, 1301, "Could not create vertex buffer")
DEFINE_EXCEPTION(CreateIndevBufferException, 1302, "Could not create index buffer")

#endif
