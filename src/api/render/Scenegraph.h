#pragma once
#include <stack>
#include "../math/matrix.h"
#include "Actor.h"
//#include "tree.h"
#include "renderer.h"

namespace FWrender {
	class Scenegraph : virtual public Referencable //, public TreeParser
	{
		public:
			Scenegraph();
			~Scenegraph();

			void Render(FWrender::Renderer & render);

		// -- tree parser implementation
		protected:
			void renderNode(FWrender::Renderer & render, Actor *& p_actor);
			void push();
			void pop();

		private:
			FWmath::Matrix m_matrix;
			std::stack<FWmath::Matrix> m_matrix_stack;

			Actor* m_pRoot;

	};
}
