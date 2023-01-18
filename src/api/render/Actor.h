#pragma once


#include "renderer.h"
#include "dxtypes.h"
#include "reference.h"
#include "shader.h"
#include "Material.h"
#include "texture.h"

#include "../math/matrix.h"

#include "../core/renderassets.h"

#include <vector>
#include <stack>

namespace Grafkit {

	class Scene;

	class Entity3DEvents;
	class Actor;

//#define ENTITY3D_BUCKET ":entity3d"

	class Entity3D //: public //Grafkit::IRenderAsset /*, virtual public Referencable*/
	{
	friend class Actor;
	public:
		Entity3D();
		virtual ~Entity3D();

		ShaderAssetRef &GetVertexShader() { return this->m_vertexShader; }
		void SetVertexShader(ShaderAssetRef shader) { this->m_vertexShader = shader; }
			
		MaterialRef &GetMaterial() { return this->m_material; }
		void SetMaterial(MaterialRef material) { this->m_material = material; }

		///@{
		///Ezek a materialbol veszik ki a shadert, ha megosztott material van, akkor mindenkiet modositja
		///Kulonben ha zero a material, akkor gaz van
		ShaderAssetRef GetFragmentShader() { return this->m_material.Valid()?this->m_material->GetShader():ShaderAssetRef(); }
		void SetFragmentShader(ShaderRef shader) { this->m_vertexShader = shader; }
		///@}

		///@todo az actor felol lehessen updatelni a shadert + a materialt

		//Actor * const & GetParent() { return m_parent; }

		virtual void Render(Grafkit::Renderer& deviceContext) = 0;

	protected:	
		//Actor* m_parent;
		ShaderAssetRef m_vertexShader;
		MaterialRef m_material;

		/// @todo + bounding box, ha kell 1

		//virtual const char* GetBucketID() { return ""; }
	};

	// ez egyszer jol jon majd
#if 0
	class Entity3DEvents {
		friend class Entity3D;
		protected:
			virtual void onUpdateShaderParams(ID3D11DeviceContext* deviceContext, Shader*& shader) {}
			virtual void onRender(ID3D11DeviceContext* deviceContext, Actor*& actor) {}
	};
#endif 

//#define ACTOR_BUCKET ":actor"

	//class Actor;
	//typedef Ref<Actor> ActorRef;

	/**
	An actor node - ez a scenegraph es a nodeja
	*/
	class Actor //: public Grafkit::IRenderAsset 
	{
	friend class Scene;
	public:
		Actor();
		~Actor();

		FWmath::Matrix& Matrix() { return m_viewMatrix; }
		virtual void Render(Grafkit::Renderer &render);

		/// @todo igazi ListTree-t hasznaljon, ha lehet, es majd mukodik
		void AddChild(Actor* child);
		Actor* GetParent() { return m_pParent; }

		std::vector<Ref<Entity3D>>& GetEntities() { return m_pEntities; }

		//virtual const char* GetBucketID() { return ACTOR_BUCKET; }

	protected:

		///@todo ezek kellenek-e?
		//void updateShader();	
		//void callDraw();		
		//ActorEvents* m_events;

		FWmath::Matrix m_viewMatrix;

		Actor* m_pParent;
		std::vector<Actor*> m_pChildren;
		std::vector<Ref<Entity3D>> m_pEntities;
	};

	//typedef Ref<Actor> ActorRef;
}
