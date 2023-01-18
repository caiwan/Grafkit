#ifndef ___TREE_H_
#define ___TREE_H_

//typedef T;

#ifndef NULL
#define NULL 0
#endif

#include "array.h"
#include "reference.h"

#define TREE_MAXDEPTH 4096

#define HAVING_SOME_SAVING_FEATURES

#ifdef HAVING_SOME_SERIALIZER_FEATURES
#include "dynamics.h"
#endif

class BinaryTree;
class ChainTree;
class ListTree;

// //////////////////////////
template <class T> class Node;
class Iterator;

// egyik node sem torlodik magatol ezert mindet torolni kell kezzel valamikor

/**
	Iteralhato node interface
*/
class Iterable : virtual public Referencable{
	friend class Iterator;

	///@todo iterator felepitese a fabol
	///@todo iterator felepitese listabol, exportalasa listaba

	protected:
		Iterable *prev, *next;
		int index;

	public:
		Iterable() : next(NULL), prev(NULL), index(-1){}
		Iterable(int _index) : next(NULL), prev(NULL), index(_index) {}

		virtual ~Iterable(){}

		inline const int getIndex() const {return this->index;}

	private:
		virtual void ___(){} //RTTI miatt
};

/**
	Iterator interfaces
*/

///@todo iterator felepitese listabol, exportalasa listaba
///@todo iterator felepitese a fabol
class Iterator{
	protected:
		Iterable* node;

		Iterable* first;
		Iterable* last;

	protected:
		int isDeleteNodes;

	public:
		Iterator() : node(NULL), first(NULL), last(NULL){}
		Iterator(Iterable *_first, Iterable *_last) : node(NULL), first(_first), last(_last){}

		virtual ~Iterator(){ if (this->isDeleteNodes && this->node) delete this->node; }

		Iterable *getNext();
		Iterable *getPrev();

		Iterable *getFirst();
		Iterable *getLast();

		inline const int hasNext() const {return this->node->next != 0;}
		inline const int hasPrev() const {return this->node->prev != 0;}

		/// @todo add operator++
		/// @todo add operator--

		/// kurrens node
		inline Iterable *get(){return this->node;}
		inline void set(Iterable * _node){this->node = _node;}

		inline const int hasNode() const {return this->node != NULL;}

		/// delete nodes in the destructor
		void deleteNodes(){isDeleteNodes = 1;}

	private:
		virtual void ___(){} //RTTI miatt
};

/**
	Encapsulates a referencable object into a node.
	**ONLY USE WHEN YOU ARE ON DRUGS**
	The bumbest solution as fuck
*/

template <class T> class Node : public Iterable{
	protected:
		Ref<T> data;

	public:
		inline Node(T *&data) : data(data){}
		inline Node(T *&data, int &index) : Iterable(index), data(data){}
		inline Node(Ref<T> &data) : data(data){}
		inline Node(Ref<T> &data, int &index) : Iterable(index), data(data){}

		inline void setData(Ref<T> data){this->data = data;}
		inline  Ref<T>& getData()  {return this->data;}
};

class NodeIterator;

/**
	Node parser (interface); 
	az Iterator minden elemet bejarja a fanak, vagy sornak, majd 
*/

///@todo virtualis tagfuggvenyeket ki kell venni
class NodeParser{
	friend class NodeIterator;
	friend class ChainTree;
	friend class BinaryTree;
	friend class ListTree;

	public:
		enum NP_search_e {
			NP_preorder = 0,
			NP_inorder,
			NP_postorder,
			NP_COUNT
		};

	protected:
		virtual void parseNode(NodeIterator* node) = 0;
		virtual void push() = 0;	///< enter subtree
		virtual void pop() = 0;		///< leave subtree
};

/**
  Iterator tipusos Node-hoz
  Binary van sima lancolt listas fahoz
*/
class NodeIterator : public Iterator{
	friend class ChainTree;
	friend class BinaryTree;
	friend class ChainTree;

	protected:
		NodeIterator *parent;
	public:
		NodeIterator() : Iterator(), parent(NULL){}
		~NodeIterator(){}

		virtual void parse(NodeParser* parser, int maxdepth) {}

		

	private:
		virtual void ___(){} //RTTI miatt

};

/*======================================================================================================================= */
class BinaryTree : public NodeIterator{
	friend class BinaryTree;

	protected:
		BinaryTree *left_child, *right_child;

		void insertChild(BinaryTree *&newChild);
		void insertNode(int isRightChild);
		BinaryTree *removeChild(BinaryTree *&oldChild);

	public:
		BinaryTree();
		~BinaryTree();

		// editing the tree
		inline void setParent(BinaryTree* parent) {this->parent = parent;}
		inline void setLeftChild(BinaryTree* child) {child->parent = this; left_child = child;}
		inline void setRightChild(BinaryTree* child) {child->parent = this; right_child = child;}

		inline bool hasLeft(){return this->left_child != NULL;}
		inline bool hasRight(){return this->right_child != NULL;}

		inline void insertLeftChild() {this->insertChild(left_child);}
		inline void insertRightChild() {this->insertChild(right_child);}

		inline void insertLeftNode() {this->insertNode(0);}
		inline void insertRightNode() {this->insertNode(1);}

		inline BinaryTree* getLeftChild(){return this->left_child;}
		inline BinaryTree* getRightChild(){return this->right_child;}

		inline BinaryTree* getParent(){return (BinaryTree*)this->parent;}

		//inline void deleteLeftChild(){delete this->left_child; this->left_child = 0;}
		//inline void deleteRightChild(){delete this->right_child; this->right_child = 0;}

		//inline BinaryTree* detachLeftChild(){return this->removeChild(this->left_child);}
		//inline BinaryTree* detachRighChild(){return this->removeChild(this->right_child);}

		void parse(NodeParser* parser, int maxdepth=TREE_MAXDEPTH);
};

/*======================================================================================================================= */

class ChainTree : public NodeIterator{
	friend class ChainTree;

	protected:
		ChainTree *child;
		ChainTree *left, *right;
		//Iterable *node;

	public:
		ChainTree();
		~ChainTree();
		
		// ---------------- // ----------------
		///@{
		/// GET Method
		inline  ChainTree* getParent() {return (ChainTree*)this->parent;}	///< @return Pointer of the parent tree. Null if root.
		inline  ChainTree* getChild() {return this->child;}	///< @return Pointer of the child. Null is there's none.
		inline  ChainTree* getLeft() {return this->left;}		///< @return Pointer of the left neighbour. Null is there's no more of them.
		inline  ChainTree* getRight() {return this->right;}	///< @return Pointer of the right neighbour	Null is there's no more of them.

		inline  int hasParent() {return this->parent != NULL;}
		inline  int hasChild() {return this->child != NULL;}
		inline  int hasLeft() {return this->left != NULL;}
		inline  int hasRight() {return this->right != NULL;}
		///@}

		// ---------------- // ----------------
		/// @{
		/// SET Method
		void setParent(ChainTree* parent);		///< @param parent new parent
		void setChild(ChainTree* child);		///< @param child new child
		void setLeft(ChainTree* left);			///< @param left new left neignour 
		void setRight(ChainTree* right);		///< @param right new right neignour 
		/// @}
	
		// ---------------- // ----------------
		/// @{
		/// INSERT Method

		void insertChild(ChainTree* _child);
		inline ChainTree* insertChild() { ChainTree *newchild = new ChainTree();  this->insertChild(newchild); return newchild;}
		
		///@todo ez hibas
		void insertLeft(ChainTree* _left);
		///@todo ez hibas
		inline ChainTree* insertLeft(){ChainTree *newleft = new ChainTree(); this->insertLeft(newleft); return newleft;}

		///@todo ez hibas
		void insertRight(ChainTree* _right);
		///@todo ez hibas
		inline ChainTree* insertRight(){ChainTree* newright= new ChainTree(); this->insertRight(newright); return newright;}

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
		void parse(NodeParser* parser, int maxdepth=TREE_MAXDEPTH);
};

/// Listas fa, a lancolt listas fa kivaltasara
/// A gyerekeket ArrayPool<T>-ben tárolja lancolt lista helyett

class ListTree : public NodeIterator{
private:
	ArrayPool<ListTree*> children;

public:
	/// @{
	/// Konstruktorok
	ListTree(int initsize = 64, int stepsize = 32);
	
	///@todo ennek a konstruktornak valami copy-konstruktornak kellene lennie, ebben a formaban igazan hasztalan
	//listTree(ListTree* parent, int initsize = 64, int stepsize = 32); ///
	/*
	ListTree(BinaryTree* binaryTree);
	ListTree(ChainTree* chainTree);
	*/

	~ListTree();
	/// @}

	// ---------------- // ----------------
	///@{
	/// GET Method
	inline  ListTree* getParent() {return (ListTree*)this->parent;} ///< @return Pointer of the parent tree. Null if root.
	inline  ListTree* getChild(int n) {return this->children[n];}	///< @return Pointer of the child. Null is there's none.
	inline  int getChildCount(){return this->children.size();}		///< @return Pointer of the child. Null is there's none.

	inline  int hasParent() {return this->parent != NULL;}
	inline  int hasChild() {return !this->children.empty();}
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
	inline ListTree* insertChild() {ListTree *next = new ListTree(); insertChild(next); return next;}

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
	inline void deleteChild(int n){delete this->children[n]; this->children[n] = NULL;}
	/// @}

	void parse(NodeParser* parser, int maxdepth=TREE_MAXDEPTH);
};



#endif /*___TREE_H_*/
