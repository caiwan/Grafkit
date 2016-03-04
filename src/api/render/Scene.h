#pragma once
#include <stack>
#include "../math/matrix.h"
#include "Actor.h"
#include "renderer.h"

namespace FWrender {
	class Scene //: virtual public Referencable
	{
	public:
		Scene();
		~Scene();

		///@ todo kamerat, es fenyforrasokat itt kezelje(?)
		///@ accoarding to Gargaj: kell egy preframe process, ami kiszuli a kamerat es a fenyeket, majd azzal rendereli le a tobbi nodeot

		void Render(FWrender::Renderer & render);

	private:
		Actor* m_pScenegraph;

		// fenyek + camera

	private:
			
	};
}
