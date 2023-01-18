#pragma once


#include "renderer.h"
#include "dxtypes.h"
#include "../core/reference.h"

#include "model.h"

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

		void setScader();
		ShaderRef &getShader();

		const Actor* getParent();

	protected:
		ShaderRef m_shader;
		const Actor* parent;

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