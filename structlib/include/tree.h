#ifndef ___TREE_H__
#define ___TREE_H__

#include <vector>

#include "node.h"
#include "iterator.h"

#define TREE_MAXDEPTH 4096

//#define HAVING_SOME_SERIALIZER_FEATURES

#ifdef HAVING_SOME_SERIALIZER_FEATURES
#include "dynamics.h"
#endif

class TreeNode;
class BinaryTree;
class ChainTree;
class ListTree;

enum NP_searchMode_e {
	NP_search_preorder = 0,
	NP_search_inorder,
	NP_search_postorder,
	NP_search_COUNT
};

/**
	Node parser (interface); 
	az Iterator minden elemet bejarja a fanak, vagy sornak, majd TreeParser
*/

class TreeParser
{
	friend class TreeNode;
	friend class ChainTree;
	friend class BinaryTree;
	friend class ListTree;

	public:
		TreeParser() {}
		virtual ~TreeParser() {}

	protected:
		virtual void parseNode(TreeNode* node) = 0;
		virtual void push() = 0;	///< enter subtree
		virtual void pop() = 0;		///< leave subtree
};

/**
  Tree node, minden tree tipus ebbol jon. Egy darab parentet ismer.
*/
class TreeNode : public Node
{
	friend class ChainTree;
	friend class BinaryTree;
	friend class ChainTree;

	public:
		TreeNode(TreeNode *parent = nullptr) : Node(), m_pParentNode(parent) {}
		virtual ~TreeNode() {}

	public:
		//implementation of Iterable Interafce
		virtual Iterator* getIterator();
		Iterator* getIterator(enum NP_searchMode_e mode);

		bool hasParent() { return this->m_pParentNode != nullptr; }
		
	public:
		virtual void parse(TreeParser* parser, enum NP_searchMode_e mode, int maxdepth);

	protected:
		TreeNode *m_pParentNode;
};

/*======================================================================================================================= */
class BinaryTree : public TreeNode
{
	friend class BinaryTree;
public:
		BinaryTree(BinaryTree* parent = nullptr) : TreeNode(parent) {}
		~BinaryTree() {}

		// editing the tree
		void setParent(BinaryTree* parent);
		void setLeftChild(BinaryTree* child);
		void setRightChild(BinaryTree* child);

		bool hasLeft() { return this->m_pLeftChild != nullptr;}
		bool hasRight() { return this->m_pRightChild != nullptr;}

		BinaryTree* insertLeftChild()  {return insertChild(m_pLeftChild);}
		BinaryTree* insertRightChild() {return insertChild(m_pRightChild);}
				  
		BinaryTree* getLeftChild()  { return this->m_pLeftChild; }
		BinaryTree* getRightChild() { return this->m_pRightChild; }
				  
		BinaryTree* getParent() { return (BinaryTree*)this->m_pParentNode; }

		///@todo add remove features somehow 

		BinaryTree* detachLeftChild() {return this->detachChild(this->m_pLeftChild);}
		BinaryTree* detachRighChild() {return this->detachChild(this->m_pRightChild);}

		virtual void parse(TreeParser* parser, enum NP_searchMode_e mode, int maxdepth = TREE_MAXDEPTH);

	protected:
		BinaryTree* m_pLeftChild, *m_pRightChild;
				  
		BinaryTree* insertChild(BinaryTree* &childNode);
		BinaryTree* detachChild(BinaryTree* &oldChild);
};

/*======================================================================================================================= */

class ChainTree : public TreeNode
{
	friend class ChainTree;
	public:
		ChainTree(ChainTree* parent = nullptr) : TreeNode(parent) {}
		~ChainTree() {}
		
		// ---------------- // ----------------
		///@{
		/// GET Method
		ChainTree* getParent() {return (ChainTree*)this->m_pParentNode;}	///< @return Pointer of the parent tree. Null if root.
		ChainTree* getChild() {return this->m_pChild;}	///< @return Pointer of the child. Null is there's none.
		ChainTree* getLeft() {return this->m_pLeft;}		///< @return Pointer of the left neighbour. Null is there's no more of them.
		ChainTree* getRight() {return this->m_pRight;}	///< @return Pointer of the right neighbour	Null is there's no more of them.

		
		bool hasChild() { return this->m_pChild != nullptr; }
		bool hasLeft() { return this->m_pLeft != nullptr; }
		bool hasRight() { return this->m_pRight != nullptr; }
		///@}

		// ---------------- // ----------------
		/// @{
		/// SET Method
		void setParent(ChainTree*& parent);		///< @param parent new parent
		void setChild(ChainTree*& child);		///< @param child new child
		void setLeft(ChainTree*& left);			///< @param left new left neignour 
		void setRight(ChainTree*& right);		///< @param right new right neignour 
		/// @}
	
		// ---------------- // ----------------
		/// @{
		/// INSERT Method

		ChainTree* insertChild(ChainTree *child = nullptr);
		ChainTree* insertLeft(ChainTree *left = nullptr);
		ChainTree* insertRight(ChainTree *right= nullptr);

		///@todo szukseg eseten a hianyzo elemeket potplni kellene
		/// @}

		// ---------------- // ----------------
		// DETACH Method

		// ---------------- // ----------------
		// DELETE Method

		// ---------------- // ----------------
		/// @{
		/// CLONE Method
		//  (...)
		/// @}

		// ---------------- // ----------------
		/// @{
		/// DELETE Method
		//inline void deleteChild(){delete this->child; this->child = NULL;}
		//inline void deleteLeft(){delete this->left; this->left = NULL;}
		//inline void deleteRight(){delete this->right; this->right = NULL;}
		///@}

		// ---------------- // ----------------
		/// Bejaro algoritmus
		virtual void parse(TreeParser* parser, enum NP_searchMode_e mode, int maxdepth = TREE_MAXDEPTH);

	protected:
		ChainTree* insertNeighbour(ChainTree* newNode, ChainTree *& insSide, ChainTree *& parentSide);


		ChainTree* m_pChild;
		ChainTree* m_pLeft, *m_pRight;

		// far left and right childrens
		ChainTree* m_pFarLeftChild, *m_pFarRightChild;
};

/// Listas fa, a lancolt listas fa kivaltasara
/// A gyerekeket vector<T>-ben tárolja lancolt lista helyett

class ListTree : public TreeNode
{
	public:
		/// @{
		/// Konstruktorok
		ListTree(ListTree* parent = nullptr): TreeNode(parent) {}
		~ListTree() {}
		/// @}

		// ---------------- // ----------------
		///@{
		/// GET Method
		ListTree* getParent() {return (ListTree*)this->m_pParentNode;} ///< @return Pointer of the parent tree. Null if root.
		ListTree* getChild(int n) {return this->m_vChildren[n];}	///< @return Pointer of the child. Null is there's none.
		size_t getChildCount(){return this->m_vChildren.size();}		///< @return Pointer of the child. Null is there's none.

		bool hasParent() { return this->m_pParentNode != nullptr; }
		bool hasChild() {return !this->m_vChildren.empty();}
		/// @}

		// ---------------- // ----------------
		/// @{
		/// SET Method
		void setParent(ListTree* parent, int autoAddParent = 1);		///< @param parent new parent
		///@todo set helyett valami replace muvelet kell, ami kicsereli az adott al-fat az uj al-fara.
		//void setChild(int n, ListTree* child);  ///< @param child new child
		/// @}

		// ---------------- // ----------------
		/// @{
		/// INSERT Method
		void insertChild(ListTree* child);	
		//inline ListTree* insertChild() {ListTree *next = new ListTree(); insertChild(next); return next;}

		//ListTree* void insertNext(ListTree* node);
		/// @}

		// ---------------- // ----------------
		// DETACH Method

		// ---------------- // ----------------
		// DELETE Method

		// ---------------- // ----------------
		/// @{
		/// CLONE Method
		//  (...)
		/// @}

		// ---------------- // ----------------
		/// @{
		/// DELETE Method
		inline void deleteChild(int n){delete this->m_vChildren[n]; this->m_vChildren[n] = NULL;}
		/// @}

		virtual void parse(TreeParser* parser, enum NP_searchMode_e mode = NP_search_inorder, int maxdepth = TREE_MAXDEPTH);
		//void parse(TreeParser* parser, int maxdepth=TREE_MAXDEPTH);

	private:
		std::vector<ListTree*> m_vChildren;
};

#endif /*___TREE_H__*/
