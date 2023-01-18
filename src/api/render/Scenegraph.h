#pragma once
#include "tree.h"


namespace FWrender {
	class Scenegraph : virtual public Referencable, public TreeParser
	{
		public:
			Scenegraph();
			~Scenegraph();

			///@todo implement
			void Render();

		// -- tree parser implementation
		protected:
			virtual void parseNode(TreeNode* node);
			virtual void push();
			virtual void pop();

		private:
			ListTree *m_pRoot;
	};
}
