#include "tree.h"


namespace FWrender {
	class Scenegraph : virtual public Referencable, public TreeParser
	{
		public:
			Scenegraph();
			~Scenegraph();

		// -- tree parser implementation
		protected:
			virtual void parseNode(TreeNode* node) = 0;
			virtual void push() = 0;	///< enter subtree
			virtual void pop() = 0;		///< leave subtree

		private:
			ListTree *m_pRoot;
	};
}