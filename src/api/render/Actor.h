#pragma once

#include <vector>
#include <stack>

#include "../utils/reference.h"
#include "../utils/resource.h"

#include "../math/matrix.h"

#include "renderer.h"
#include "shader.h"
#include "Material.h"


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
		virtual void Render(Grafkit::Renderer& deviceContext, Scene* scene) = 0;
		
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
		Grafkit::Matrix& Transform() { return m_transformMatrix; }

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
		Grafkit::Matrix m_transformMatrix; // transzformacio a view matrix teterjen

		Ref<Actor> m_pParent;
		std::vector<Ref<Actor>> m_pChildren;
		std::vector<Ref<Entity3D>> m_pEntities;
	};

}
