#include "tree.h"

#include <cstring>
#include <cstdlib>


Iterable *Iterator::getNext()
{
	Iterable *curr = this->node;
	this->node = curr->next;

	return curr;
}

Iterable *Iterator::getPrev()
{
	Iterable *curr = this->node;
	this->node = curr->prev;

	return curr;
}

// ------------------------------------------------------------------------

// implementation
BinaryTree::BinaryTree() : NodeIterator() {
	//this->node_data = NULL;
	this->parent = NULL;
	this->left_child = NULL;
	this->right_child = NULL;
}

BinaryTree::~BinaryTree(){
	if (isDeleteNodes){
		if (this->left_child) this->left_child->deleteNodes();
		if (this->right_child) this->right_child->deleteNodes();
	}
	delete this->left_child;
	delete this->right_child;
}

// editing
void BinaryTree::insertChild(BinaryTree *&newChild){
	//if (newChild){
		// ... 
	//}else{
		newChild = new BinaryTree();
		newChild->parent = this;
	//}
}

void BinaryTree::insertNode(int b){
	BinaryTree *newNode = new BinaryTree();
	newNode->parent = this;
	if (b){
		this->right_child->parent = newNode;
		newNode->right_child = this->right_child;
		this->right_child = newNode;
	} else {
		this->left_child->parent = newNode;
		newNode->left_child = this->left_child;
		this->left_child = newNode;	
	}
}

BinaryTree* BinaryTree::removeChild(BinaryTree *&oldChild){
	BinaryTree* tmp = oldChild; 
	tmp->parent = NULL; 
	this->right_child = NULL; 
	return tmp;
}

void BinaryTree::parse(NodeParser* parser, int maxdepth){
	if(!parser) return;
	if(maxdepth<0) return;

	///@todo fa bejarasi strategiak (inorder, preorder, ... +inverz)

	parser->push();

	// kurrens node
	if (this->hasNode()) parser->parseNode(this);

	//balra
	if(this->left_child) this->left_child->parse(parser, maxdepth-1);

	//jobbra
	if(this->right_child) this->right_child->parse(parser, maxdepth-1);

	parser->pop();
}

// ------------------------------------------------------------------------


ChainTree::ChainTree()
	: NodeIterator()
{
	this->parent = NULL;
	this->child = NULL;
	this->left = NULL;
	this->right = NULL;
}

ChainTree::~ChainTree()
{
	if (isDeleteNodes){
		if (this->left) this->left->deleteNodes();
		if (this->right) this->right->deleteNodes();
		if (this->child) this->child->deleteNodes();
	}

	delete this->left;
	delete this->right;
	delete this->child;
}

// -------- 

#define CHECKIF(x) if(!x) return

void ChainTree::setParent(ChainTree* _parent){
	this->parent = _parent; 
	//CHECKIF(_parent);		//ez tonkreteszi a fat
	//this->parent->child = this;
}	

void ChainTree::setChild(ChainTree* _child){
	this->child = _child; 
	CHECKIF(_child);
	///@todo bug: First-chance exception
	this->child->parent = this;
}	

void ChainTree::setLeft(ChainTree *_left){
	this->left = _left;
	CHECKIF(_left);
	this->left->right = this;
}

void ChainTree::setRight(ChainTree *_right){
	this->right = _right;

	CHECKIF(_right);
	this->right->left = this;
}

// -------- 

/// @todo nem szur be rendesen
void ChainTree::insertChild(ChainTree *_child){
	//CHECKIF(_child);	
	///@todo lehessen null is -> biztonsagos(abb) null seteles
	if(!this->child){
		setChild(_child);
	} else {
		///@todo keresse meg az uj fa utolso childejat, es oda illessze be
		_child->setChild(this->child);
		setChild(_child);
	}
}

void ChainTree::insertLeft(ChainTree *_left){
	///@todo lehessen null is -> biztonsagos(abb) null seteles
	_left->setParent((ChainTree*)this->parent);
	if(!this->left){ 
		setLeft(_left);
	} else {
		///@todo keresse meg az uj fa utolso elemet a listaban, es oda illessze be
		_left->setLeft(this);
		this->setLeft(_left);
	}
}

void ChainTree::insertRight(ChainTree *_right){
	///@todo lehessen null is -> biztonsagos(abb) null seteles
	_right->setParent((ChainTree*)this->parent);
	if(!this->right){ 
		this->right =_right;
	} else {
		///@todo keresse meg az uj fa utolso elemet a listaban, es oda illessze be
		_right->setRight(this);
		this->setRight(_right);
	}
}

void ChainTree::parse(NodeParser* parser, int maxdepth){
	if(!parser) return;
	if(maxdepth<0) return;

	///@todo fa bejarasi strategiak

	parser->push();

	// kurrens node feldolg
	if(this->hasNode()) parser->parseNode(this);

	// kozvetlen leszarmazott
	if(this->hasChild()) child->parse(parser, maxdepth-1);

	///@todo feltelezezzuk, hogy nincs balra szomszed -> parseolas elott rendezni kell a fat, hogy a kozvetlen leszarmazott legyen a szelso-baloldali elem
	// innentol csak jobbra indulunk

	parser->pop();

	if(this->hasRight()) right->parse(parser, maxdepth-1);
}

// ------------------------------------------------------------------------

ListTree::ListTree(int initsize, int stepsize)
	: NodeIterator(),
	children(initsize, stepsize)
{

}

/*
ListTree::ListTree(ListTree *_parent, int initsize, int stepsize)
	: children(initsize, stepsize)
{
	this->parent = _parent;
	this->parent->insertChild(this);
}
*/
	/*
ListTree::ListTree(BinaryTree *binaryTree)
	: NodeIterator(),
	children(1,1)
{
	// copy
	///@todo implement
}

ListTree::ListTree(ChainTree *chainTree)
	: NodeIterator(),
	children(1,1)
	
{
	// copy
	///@todo implement
}
*/

ListTree::~ListTree()
{
	
	for (int i=0; i<this->getChildCount(); i++){
		if (isDeleteNodes && this->children[i]) this->children[i]->deleteNodes();
		delete this->children[i];
	}
}

void ListTree::setParent(ListTree *parent, int autoAddParent)
{
	/*
	if(autoAddParent){
		parent->insertChild(this);
	}
	*/
	this->parent = parent;
}

void ListTree::insertChild(ListTree *child)
{
	this->children << child;
	child->setParent(this, 0);
}

void ListTree::parse(NodeParser *parser, int maxdepth)
{
	if(!parser) return;
	if(maxdepth<0) return;

	///@todo fa bejarasi strategiak

	parser->push();

	// kurrens node feldolg
	if(this->hasNode()) parser->parseNode(this);

	// kozvetlen leszarmazott
	if(this->hasChild()) {
		for(int i=0; i<this->getChildCount(); i++){
			this->children[i]->parse(parser, maxdepth-1);
		}
	}

	parser->pop();
}

