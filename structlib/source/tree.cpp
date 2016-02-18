#include <cstring>
#include <cstdlib>

#include "tree.h"
#include "exceptions.h"

/**
*/

using namespace FWdebugExceptions;

// ----- 

namespace GenIterator{
	class Parser : public TreeNode {
		Parser(){}
		~Parser() {}
	};
}

Iterator * TreeNode::getIterator()
{
	return this->getIterator(NP_search_preorder);
}

Iterator * TreeNode::getIterator(NP_searchMode_e mode)
{
	TreeNode *gen = nullptr;

	/// @todo ... 

	return nullptr;
}

void TreeNode::parse(TreeParser * parser, NP_searchMode_e mode, int maxdepth)
{
	throw EX(InvalidOperationException);
}


// ====================================================================================================================================================================================
// Implementation of BinaryTree
// ====================================================================================================================================================================================

// editing
BinaryTree * BinaryTree::insertChild(BinaryTree * &childNode){
	if (childNode!= nullptr){
		throw EX_DETAILS(NotImplementedMethodException, "Ket elem koze torteno beszuras meg nincs kesz");
	}else{
		childNode = new BinaryTree(this);
	}

	return childNode;
}

BinaryTree * BinaryTree::detachChild(BinaryTree * & oldChild)
{
	BinaryTree * oldChildCopy = oldChild;
	oldChildCopy->setParent(nullptr);
	oldChild = nullptr;
	return oldChildCopy;
}

void BinaryTree::setParent(BinaryTree * parent)
{
	/// @todo refcount, ha kell 
	this->m_pParentNode = parent;
}

void BinaryTree::setLeftChild(BinaryTree * child)
{
	/// @todo refcount, ha kell 
	this->m_pLeftChild = child;
}

void BinaryTree::setRightChild(BinaryTree * child)
{
	/// @todo refcount, ha kell 
	this->m_pRightChild = child;
}


//void BinaryTree::removeNode()
//{
//	/// @todo refcount, ha kell 
//	// isten bassza meg 
//	throw EX_DETAILS(NotImplementedMethodException, "az isten bassza meg, oke");
//}

void BinaryTree::parse(TreeParser* parser, enum NP_searchMode_e mode, int maxdepth) {
	if(!parser) return;
	if(maxdepth<0) return;

	///@todo fa bejarasi strategiak (inorder, preorder, ... +inverz)

	parser->push();

	///@ todo ezeket a bejaro fuggvenyeket ki kene venni inkabb kulon metodusokba
	switch (mode) {
	case NP_search_inorder: // visit, left, right
		if (this->hasNode()) parser->parseNode(this);
		if (this->m_pLeftChild) this->m_pLeftChild->parse(parser, mode, maxdepth - 1);
		if (this->m_pRightChild) this->m_pRightChild->parse(parser, mode, maxdepth - 1);
		break;
	
	case NP_search_preorder: // left, visit, right
		if (this->m_pLeftChild) this->m_pLeftChild->parse(parser, mode, maxdepth - 1);
		if (this->hasNode()) parser->parseNode(this);
		if (this->m_pRightChild) this->m_pRightChild->parse(parser, mode, maxdepth - 1);
		break;
	
	case NP_search_postorder: // left, right, visit
		if (this->m_pLeftChild) this->m_pLeftChild->parse(parser, mode, maxdepth - 1);
		if (this->m_pRightChild) this->m_pRightChild->parse(parser, mode, maxdepth - 1);
		if (this->hasNode()) parser->parseNode(this);
		break;
	}

	parser->pop();
}

// ====================================================================================================================================================================================
// Implementation of ChainTree
// ====================================================================================================================================================================================

void ChainTree::setParent(ChainTree *&parent){
	if (parent)
		parent->m_pChild = this;
	this->m_pParentNode = parent;
}	

void ChainTree::setChild(ChainTree *&child){
	if (child)
		child->m_pParentNode = this;
	this->m_pChild = child;
}	

void ChainTree::setLeft(ChainTree *&left){
	if (left)
		left->m_pRight = this;
	this->m_pLeft = left;
}

void ChainTree::setRight(ChainTree *&right){
	if (right)
		right->m_pLeft = this;
	this->m_pRight = right;
}

// -------- 

/// @todo nem szur be rendesen
ChainTree* ChainTree::insertChild(ChainTree *child){
	ChainTree *newchild = child;
	if (newchild== nullptr) {
		newchild = new ChainTree(this);
	}

	this->m_pChild = newchild;
	this->m_pFarRightChild->insertRight(newchild);
	this->m_pFarRightChild = newchild;
	
	// is it was the first element
	if (this->m_pFarLeftChild== nullptr)
		this->m_pFarLeftChild = newchild;

	return newchild;
}

ChainTree* ChainTree::insertNeighbour(ChainTree* _newNode, ChainTree*& insSide, ChainTree*&parentSide)
{
	if (m_pParentNode == nullptr) {
		throw EX_DETAILS(InvalidOperationException, "A node without valid root can not have neighbours");
	}

	ChainTree*newNode = _newNode;
	if (newNode== nullptr) {
		newNode = new ChainTree();
	}

	newNode->m_pRight = this;
	newNode->m_pLeft = insSide;

	insSide->m_pLeft = newNode;
	insSide = newNode;

	// check if parent's far left or frar right 
	ChainTree *&farSide = parentSide;
	if (farSide == nullptr || farSide == this)
	{
		farSide = newNode;
	}
}

inline ChainTree* ChainTree::insertLeft(ChainTree *left) {
	return insertNeighbour(left, m_pLeft, this->getParent()->m_pFarLeftChild);
}

inline ChainTree* ChainTree::insertRight(ChainTree *right){
	return insertNeighbour(right, m_pRight, this->getParent()->m_pFarRightChild);
}

void ChainTree::parse(TreeParser* parser, enum NP_searchMode_e mode, int maxdepth) {
	if (!parser) return;
	if (maxdepth < 0) return;

	///@ todo ezeket a bejaro fuggvenyeket ki kene venni inkabb kulon metodusokba
	///@todo feltelezezzuk, hogy nincs balra szomszed -> parseolas elott rendezni kell a fat, hogy a kozvetlen leszarmazott legyen a szelso-baloldali elem
	// innentol csak jobbra indulunk

	if (mode == NP_search_inorder) {
		parser->push();
		if (this->hasNode()) parser->parseNode(this);
		if (this->hasChild()) m_pChild->parse(parser, mode, maxdepth - 1);

		parser->pop();

		if (this->hasRight()) m_pRight->parse(parser, mode, maxdepth - 1);
	}
	else {
		parser->push();
		if (this->hasChild()) m_pChild->parse(parser, mode, maxdepth - 1);
		if (this->hasNode()) parser->parseNode(this);

		parser->pop();

		if (this->hasRight()) m_pRight->parse(parser, mode, maxdepth - 1);
	}
}

// ====================================================================================================================================================================================
// Implementation of ListTree
// ====================================================================================================================================================================================

void ListTree::setParent(ListTree *parent, int autoAddParent)
{
	this->m_pParentNode = parent;
}

void ListTree::insertChild(ListTree *child)
{
	this->m_vChildren.push_back(child);
	child->setParent(this, 0);
}

void ListTree::parse(TreeParser* parser, enum NP_searchMode_e mode, int maxdepth) {
	if(!parser) return;
	if(maxdepth<0) return;

	///@todo fa bejarasi strategiak

	parser->push();

	///@ todo ezeket a bejaro fuggvenyeket ki kene venni inkabb kulon metodusokba

	//	inorder:= parse, majd gyerek, kulonben forditva
	if (mode == NP_search_inorder) {
		// kurrens node feldolg

		if (this->hasNode())
			parser->parseNode(this);

		// kozvetlen leszarmazott
		if (this->hasChild()) {
			for (int i = 0; i < this->getChildCount(); i++) {
				this->m_vChildren[i]->parse(parser, mode, maxdepth - 1);
			}
		}
	}
	else
	{
		
		if (this->hasChild()) {
			for (int i = 0; i < this->getChildCount(); i++) {
				this->m_vChildren[i]->parse(parser, mode, maxdepth - 1);
			}
		}

		if (this->hasNode())
			parser->parseNode(this);
	}

	parser->pop();
}
