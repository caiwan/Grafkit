#pragma once


#include "renderer.h"
#include "dxtypes.h"
#include "reference.h"
#include "shader.h"
#include "Material.h"
#include "texture.h"

#include <vector>

namespace FWrender {

	class Entity3DEvents;
	class Actor;

	class Entity3D : public virtual Referencable
	{
		friend class Actor;
		public:
			Entity3D();
			virtual ~Entity3D();

			ShaderRef &GetVertexShader() { return this->m_vertexShader; }
			TextureRef &GetTexture(int n = 0) { return m_textures[n]; } ///@todo bound check
			MaterialRef &GetMaterial() { this->m_material; }

			Actor * const & GetParent() { return m_parent; }

		protected:	
			Actor* m_parent;

			ShaderRef m_vertexShader;

			MaterialRef m_material;
			TextureRef m_textures[MULTITEXTURE_MAX];

			/// @todo + transformation
			/// @todo + bounding box, ha kell 
	};


	class Entity3DEvents {
		friend class Entity3D;
		protected:
			virtual void onUpdateShaderParams(ID3D11DeviceContext* deviceContext, Shader*& shader) {}
			virtual void onRender(ID3D11DeviceContext* deviceContext, Actor*& actor) {}
	};


	/**
	An actor node 
	Todo: advanced architectural design goez here 
	*/
	class Actor : public virtual Referencable
	{

		public:
			Actor();
			Actor(const Actor&);
			~Actor();

			virtual void Render(ID3D11DeviceContext* deviceContext) = 0;

		protected:
			//void updateShader();	///@todo implement
			//void callDraw();		///@todo implement

			//ActorEvents* m_events;

			/// @todo + transformation

			std::vector<Ref<Entity3D>> entities;
	};

	typedef Ref<Actor> ActorRef;
}