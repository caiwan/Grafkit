#pragma once
#include <stack>
#include "../math/matrix.h"
#include "tree.h"
#include "renderer.h"

namespace FWrender {
	class Scenegraph : virtual public Referencable, public TreeParser
	{
		public:
			Scenegraph();
			~Scenegraph();

			///@todo implement
			void Render(FWrender::Renderer & render);

		// -- tree parser implementation
		protected:
			virtual void parseNode(TreeNode* node);
			virtual void push();
			virtual void pop();

		private:
			FWmath::Matrix m_matrix;
			std::stack<FWmath::Matrix> m_matrix_stack;

			ListTree *m_pRoot;
			FWrender::Renderer * m_render; ///@todo ezzel itt kezdeni kell valamit
	};
}
