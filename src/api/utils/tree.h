#ifndef ___TREE_H__
#define ___TREE_H__

#include <vector>
#include <stack>

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

/*======================================================================================================================= */

/**
Tree parser (interface);
Custom rekurziv fa bejarashoz; ezt az interfacet orokited, implementalod, es jo
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
	/// Visit node 
	virtual void ParseNode(TreeNode* node) = 0;

	/**
	Push node
	@param node parent node that being pushed into a stack
	*/
	virtual void Push(TreeNode* node) = 0;

	/**
	Pop node
	@param node the node that popped out of the stack
	*/
	virtual void Pop(TreeNode*) = 0;
};

/**
Tree iterator (interface);
Custom iterativ fa bejarashoz; ezt az interfacet orokited, implementalod, es jo
	- Csak eloremeno iteraciora kepes, visszafele nem tud menni - ezt meg kell oldani valahogy majd
*/
class TreeIterator : Iterator
{
	friend class TreeNode;
	friend class ChainTree;
	friend class BinaryTree;
	friend class ListTree;

public:
	TreeIterator(TreeNode* root) : Iterator(), m_parser(nullptr), m_pCurrent(nullptr), m_pRoot(root) {}
	~TreeIterator() {}

	virtual void First() { m_pCurrent = m_pRoot; };
	virtual void Last() {};		///< Not supported
	virtual void Previous() {}	///< not supported
	virtual int HasPrev() { return false; }; ///< not supported

	//virtual Node* getCurrent() { return dynamic_cast<Node*>(m_pCurrent); }
	virtual Node* GetCurrent() { return (Node*)(m_pCurrent); } ///@todo a picaert nem megy a static vagy dynamic cast

	void SetParser(TreeParser* parser) { this->m_parser = parser; }

	// ezeket kell implementalni:
	virtual int IsDone() = 0;
	virtual void Next() = 0;
	virtual int HasNext() = 0;

protected:
	TreeNode *m_pRoot, *m_pCurrent;
	TreeParser* m_parser;
	std::stack<TreeNode*> m_sNode;
};



/*======================================================================================================================= */

/** milyen tipusu bejaro objektumot hozzon letre
*/
enum TRAVELSAL_Gen_Type {
	TREE_TRAVEL_preorder = 0,
	TREE_TRAVEL_inorder,
	TREE_TRAVEL_postorder,
	TREE_TRAVEL_COUNT
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

		///Alabol preorder iteratort hoz letre
		virtual Iterator* GetIterator();
		virtual Iterator* GetIterator(enum TRAVELSAL_Gen_Type mode) = 0;

		bool HasParent() { return this->m_pParentNode != nullptr; }

	protected:
		TreeNode *m_pParentNode;
};

/*======================================================================================================================= */
/**
*/
class BinaryTree : public TreeNode
{
friend class BinaryTree;
public:
	BinaryTree(BinaryTree* parent = nullptr) : TreeNode(parent) {}
	~BinaryTree() {}

	// editing the tree
	void SetParent(BinaryTree* parent);
	void SetLeftChild(BinaryTree* child);
	void SetRightChild(BinaryTree* child);

	bool HasLeft() { return this->m_pLeftChild != nullptr;}
	bool HasRight() { return this->m_pRightChild != nullptr;}

	BinaryTree* InsertLeftChild()  {return InsertChild(m_pLeftChild);}
	BinaryTree* InsertRightChild() {return InsertChild(m_pRightChild);}
				  
	BinaryTree* GetLeftChild()  { return this->m_pLeftChild; }
	BinaryTree* GetRightChild() { return this->m_pRightChild; }
				  
	BinaryTree* GetParent() { return (BinaryTree*)this->m_pParentNode; }

	///@todo add remove features somehow 

	BinaryTree* DetachLeftChild() {return this->DetachChild(this->m_pLeftChild);}
	BinaryTree* DetachRighChild() {return this->DetachChild(this->m_pRightChild);}

	virtual Iterator* GetIterator(enum TRAVELSAL_Gen_Type mode = TREE_TRAVEL_preorder);

protected:
	BinaryTree* m_pLeftChild, *m_pRightChild;
				  
	BinaryTree* InsertChild(BinaryTree* &childNode);
	BinaryTree* DetachChild(BinaryTree* &oldChild);

	protected:
		class PreorderIterator : public TreeIterator {
		public:
			PreorderIterator(BinaryTree* root) : TreeIterator(root){}
			~PreorderIterator() {}
			
			virtual int IsDone();
			virtual void Next(); 
			virtual int HasNext();
		};
		
		/*class InorderIterator : public TreeIterator {
		};*/
		class PostorderIterator : public TreeIterator {
		};
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
		ChainTree* GetParent() {return (ChainTree*)this->m_pParentNode;}	///< @return Pointer of the parent tree. Null if root.
		ChainTree* GetChild() {return this->m_pChild;}	///< @return Pointer of the child. Null is there's none.
		ChainTree* GetLeft() {return this->m_pLeft;}		///< @return Pointer of the left neighbour. Null is there's no more of them.
		ChainTree* GetRight() {return this->m_pRight;}	///< @return Pointer of the right neighbour	Null is there's no more of them.

		
		bool HasChild() { return this->m_pChild != nullptr; }
		bool HasLeft() { return this->m_pLeft != nullptr; }
		bool HasRight() { return this->m_pRight != nullptr; }
		///@}

		// ---------------- // ----------------
		/// @{
		/// SET Method
		void SetParent(ChainTree*& parent);		///< @param parent new parent
		void SetChild(ChainTree*& child);		///< @param child new child
		void SetLeft(ChainTree*& left);			///< @param left new left neignour 
		void SetRight(ChainTree*& right);		///< @param right new right neignour 
		/// @}
	
		// ---------------- // ----------------
		/// @{
		/// INSERT Method

		ChainTree* InsertChild(ChainTree *child = nullptr);
		ChainTree* InsertLeft(ChainTree *left = nullptr);
		ChainTree* InsertRight(ChainTree *right= nullptr);

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
		virtual Iterator* GetIterator(enum TRAVELSAL_Gen_Type mode);

	protected:
		ChainTree* InsertNeighbour(ChainTree* newNode, ChainTree *& insSide, ChainTree *& parentSide);


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
		ListTree* GetParent() {return (ListTree*)this->m_pParentNode;} ///< @return Pointer of the parent tree. Null if root.
		ListTree* GetChild(int n) {return this->m_vChildren[n];}	///< @return Pointer of the child. Null is there's none.
		size_t GetChildCount(){return this->m_vChildren.size();}		///< @return Pointer of the child. Null is there's none.

		bool HasParent() { return this->m_pParentNode != nullptr; }
		bool HasChild() {return !this->m_vChildren.empty();}
		/// @}

		// ---------------- // ----------------
		/// @{
		/// SET Method
		void SetParent(ListTree* parent, int autoAddParent = 1);		///< @param parent new parent
		///@todo set helyett valami replace muvelet kell, ami kicsereli az adott al-fat az uj al-fara.
		//void setChild(int n, ListTree* child);  ///< @param child new child
		/// @}

		// ---------------- // ----------------
		/// @{
		/// INSERT Method
		void InsertChild(ListTree* child);	
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
		inline void DeleteChild(int n){delete this->m_vChildren[n]; this->m_vChildren[n] = nullptr;}
		/// @}

		virtual Iterator* GetIterator(enum TRAVELSAL_Gen_Type mode);

	private:
		std::vector<ListTree*> m_vChildren;
};





#endif /*___TREE_H__*/
