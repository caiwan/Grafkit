#pragma once


#include "renderer.h"
#include "dxtypes.h"
#include "../core/reference.h"

#include "model.h"

#include <vector>

namespace FWrender {

	/**
	Stores extended data for a mesh, that makes it a model
	*/
	class Actor : public virtual Referencable
	{
	public:
		Actor();
		Actor(const Actor&);
		~Actor();


		///@todo at kell venni a hieroglyphbol 
		///@todo get rid of this
		//bool Initialize(
		//	ID3D11Device* device,
		//	FWrender::TextureRef texture,
		//	int indicesCount, const int* indices,
		//	int verticesCount, const float3* position, const float2* uv
		//	) {
		//	/// @TODO make it nice and juicy - have some exception
		//	/// @todo ez nem kell 
		//	//CreateVertexBuffer(device, verticesCount, position, uv);
		//	//CreateIndexBuffer(device, indicesCount, indices);
		//	SetTexture(texture);

		//	return true;
		//}

		void Render(ID3D11DeviceContext* deviceContext);

		void SetTexture(FWrender::TextureRef texture) { this->m_texture = texture; }
		ID3D11ShaderResourceView* GetTexture();

	private:
		inline void Shutdown();
		
	private:
		FWrender::TextureRef m_texture;
		FWrender::ShaderRef m_shader;
		std::vector<FWrender::MeshRef> m_meshes;
	};

}