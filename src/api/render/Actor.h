#pragma once


#include "renderer.h"
#include "dxtypes.h"
#include "../core/reference.h"

#include "model.h"

#include <vector>

namespace FWrender {

	/**
	Technikailag barmi lehet ez, ami a scenegraphba nodekent bekerul
	- model
	- light

	*/
	class Actor : public virtual Referencable
	{
	public:
		Actor();
		Actor(const Actor&);
		~Actor();

		/*
		void addMesh();
		void addTexture();
		void setTexture();
		void setMaterial();
		*/

		void Render(ID3D11DeviceContext* deviceContext);

		/*
		void SetTexture(FWrender::TextureRef texture) { this->m_texture = texture; }
		ID3D11ShaderResourceView* GetTexture();
		*/

	private:
		inline void Shutdown();

	private:
		FWrender::TextureRef m_texture;
		FWrender::ShaderRef m_shader;
		std::vector<FWrender::MeshRef> m_meshes;
	};


	/**
	Stores extended data for a mesh, that makes it a model
	*/
	class Model : public virtual Referencable {


	};
}