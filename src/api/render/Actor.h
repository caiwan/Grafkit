#pragma once

#include "reference.h"

#include "renderer.h"
#include "shader.h"
#include "Material.h"
// #include "texture.h"


#include "../math/matrix.h"
#include "../core/resource.h"

#include <vector>
#include <stack>

namespace Grafkit {

	class Scene;

	class Entity3DEvents;
	class Actor;



#define ENTITY3D_BUCKET "empty"

	class Entity3D : public Grafkit::IResource
	{
	friend class Actor;
	public:
		Entity3D();
		virtual ~Entity3D();

		/*ShaderResRef &GetVertexShader() { return this->m_vertexShader; }
		void SetVertexShader(ShaderResRef shader) { this->m_vertexShader = shader; }*/
			
		// MaterialRef &GetMaterial() { return this->m_material; }
		// void SetMaterial(MaterialRef material) { this->m_material = material; }

		///@{
		///Ezek a materialbol veszik ki a shadert, ha megosztott material van, akkor mindenkiet modositja
		///Kulonben ha zero a material, akkor gaz van
		// ShaderResRef GetFragmentShader() { return this->m_fragmentShader; }
		// void SetFragmentShader(ShaderResRef shader) { this->m_fragmentShader = shader; }
		///@}

		///@todo az actor felol lehessen updatelni a shadert + a materialt

		//Actor * const & GetParent() { return m_parent; }

		virtual void Render(Grafkit::Renderer& deviceContext, Scene* scene) = 0;
		
	protected:	
		//Actor* m_parent;
		// ShaderResRef m_vertexShader;
		// ShaderResRef m_fragmentShader;

		//MaterialRef m_material;

		/// @todo + bounding box, ha kell 1 (Axis aligned bounding box)

		virtual const char* GetBucketID() { return ENTITY3D_BUCKET; }
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

#define ACTOR_BUCKET "actor"

	class Actor;
	typedef Ref<Actor> ActorRef;

	/**
	An actor node - ez a scenegraph es a nodeja
	*/
	__declspec(align(16)) class Actor : public Grafkit::IResource, public AlignedNew<Actor>
	// class Actor : public Grafkit::IResource 
	{
	friend class Scene;
	public:
		Actor();
		~Actor();

		Grafkit::Matrix& Matrix() { return m_viewMatrix; }
		virtual void Render(Grafkit::Renderer &render, Scene* scene);

		/// @todo igazi ListTree-t hasznaljon, ha lehet, es majd mukodik
		void AddChild(Actor* child);
		Ref<Actor> GetParent() { return m_pParent; }

		void AddEntity(Ref<Entity3D> entity) { m_pEntities.push_back(entity); }

		std::vector<Ref<Entity3D>>& GetEntities() { return m_pEntities; }

		virtual const char* GetBucketID() { return ACTOR_BUCKET; }

	protected:

		///@todo ezek kellenek-e?
		//void updateShader();	
		//void callDraw();		
		//ActorEvents* m_events;

		Grafkit::Matrix m_viewMatrix;

		Ref<Actor> m_pParent;
		std::vector<Ref<Actor>> m_pChildren;
		std::vector<Ref<Entity3D>> m_pEntities;
	};

}
