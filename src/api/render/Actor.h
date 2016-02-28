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

namespace FWrender {

	class Scene;

	class Entity3DEvents;
	class Actor;

	class Entity3D : public virtual Referencable, virtual public FWassets::IRenderAsset
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

		virtual void Render(FWrender::Renderer& deviceContext) = 0;

	protected:	
		//Actor* m_parent;
		ShaderAssetRef m_vertexShader;
		MaterialRef m_material;

		/// @todo + bounding box, ha kell 1

		virtual enum RA_type_e GetBucketID() { return FWassets::IRenderAsset::RA_TYPE_Entity3D; }
	};


	class Entity3DEvents {
		friend class Entity3D;
		protected:
			virtual void onUpdateShaderParams(ID3D11DeviceContext* deviceContext, Shader*& shader) {}
			virtual void onRender(ID3D11DeviceContext* deviceContext, Actor*& actor) {}
	};


	/**
	An actor node - ez a scenegraph es a nodeja
	*/
	class Actor //: public virtual Referencable
	{
	friend class Scene;
	public:
		Actor();
		~Actor();

		FWmath::Matrix& Matrix() { return m_viewMatrix; };

		virtual void Render(FWrender::Renderer &render);

	private:
		void push();
		void pop();

		FWmath::Matrix m_viewMatrix;	///< aktualis modelnezeti matzrix
		std::stack<FWmath::Matrix> m_matrixStack; ///< matrix stack

	protected:
		///@todo ezek kellenek-e?
		//void updateShader();	///@todo implement
		//void callDraw();		///@todo implement
		//ActorEvents* m_events;

		FWmath::Matrix m_viewMatrix;

		Actor* m_pParent;
		std::vector<Actor*> m_pChildren;
		std::vector<Ref<Entity3D>> m_pEntities;
	};

	//typedef Ref<Actor> ActorRef;
}
