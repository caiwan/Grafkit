#pragma once


#include "renderer.h"
#include "dxtypes.h"
#include "../core/reference.h"

#include "model.h"

#include <vector>

namespace FWrender {

	class ActorEvents;

	/**
	Technikailag barmi lehet ez, ami a scenegraphba nodekent bekerul
	- model
	- light
	- ...etc 

	*/
	class Actor : public virtual Referencable
	{
		/** @todo 
		A jovoben ezt jobban el kell valasztani. 
			- Elem (3DEntity) - ez barmilyen 3d elem, ami a terben renderelodik, alapvetoen Mesh, Camera es Light. 
			- Csomopont (3DNode) - ez az, ami a scenegraph csomopontban talalhato - imho nem kell ennyire pontosan szetvalogatni
			- Az actor csak hivatkozik (pointer) ezekre az elemekre
		*/
	public:
		Actor();
		Actor(const Actor&);
		~Actor();

		void setEventCallbacks(ActorEvents* events) { m_events = events; }
		virtual void Render(ID3D11DeviceContext* deviceContext) = 0;

	protected:
		void updateShader();	///@todo implement
		void callDraw();		///@todo implement

		ActorEvents* m_events;
		ShaderRef m_shader;

	};

	class ActorEvents {
		friend class Actor;
	protected:
		virtual void onUpdateShaderParams(ID3D11DeviceContext* deviceContext, Shader*& shader){}
		virtual void onRender(ID3D11DeviceContext* deviceContext, Actor*& actor) {}
	};
}