#include "../stdafx.h"

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

Iterator * TreeNode::GetIterator()
{
	return this->GetIterator(TREE_TRAVEL_preorder);
}

// ====================================================================================================================================================================================
// Implementation of BinaryTree
// ====================================================================================================================================================================================

Iterator * BinaryTree::GetIterator(enum TRAVELSAL_Gen_Type mode)
{
	switch (mode) {
	case TREE_TRAVEL_preorder:
		return new BinaryTree::PreorderIterator(this);
	case TREE_TRAVEL_inorder:
		return nullptr;
	case TREE_TRAVEL_postorder:
		return nullptr;
	}
}

// editing
BinaryTree * BinaryTree::InsertChild(BinaryTree * &childNode){
	if (childNode!= nullptr){
		throw EX_DETAILS(NotImplementedMethodException, "Ket elem koze torteno beszuras meg nincs kesz");
	}else{
		childNode = new BinaryTree(this);
	}

	return childNode;
}

BinaryTree * BinaryTree::DetachChild(BinaryTree * & oldChild)
{
	BinaryTree * oldChildCopy = oldChild;
	oldChildCopy->SetParent(nullptr);
	oldChild = nullptr;
	return oldChildCopy;
}

void BinaryTree::SetParent(BinaryTree * parent)
{
	/// @todo refcount, ha kell 
	this->m_pParentNode = parent;
}

void BinaryTree::SetLeftChild(BinaryTree * child)
{
	/// @todo refcount, ha kell 
	this->m_pLeftChild = child;
}

void BinaryTree::SetRightChild(BinaryTree * child)
{
	/// @todo refcount, ha kell 
	this->m_pRightChild = child;
}


//void BinaryTree::RemoveNode()
//{
//	/// @todo refcount, ha kell 
//	// isten bassza meg 
//	throw EX_DETAILS(NotImplementedMethodException, "az isten bassza meg, oke");
//}

//void BinaryTree::parse(TreeParser* parser, enum NP_searchMode_e mode, int maxdepth) {
//	if(!parser) return;
//	if(maxdepth<0) return;
//
//	///@todo fa bejarasi strategiak (inorder, preorder, ... +inverz)
//
//	parser->push();
//
//	///@ todo ezeket a bejaro fuggvenyeket ki kene venni inkabb kulon metodusokba
//	switch (mode) {
//	case NP_search_inorder: // visit, left, right
//		if (this->HasNode()) parser->ParseNode(this);
//		if (this->m_pLeftChild) this->m_pLeftChild->parse(parser, mode, maxdepth - 1);
//		if (this->m_pRightChild) this->m_pRightChild->parse(parser, mode, maxdepth - 1);
//		break;
//	
//	case NP_search_preorder: // left, visit, right
//		if (this->m_pLeftChild) this->m_pLeftChild->parse(parser, mode, maxdepth - 1);
//		if (this->HasNode()) parser->ParseNode(this);
//		if (this->m_pRightChild) this->m_pRightChild->parse(parser, mode, maxdepth - 1);
//		break;
//	
//	case NP_search_postorder: // left, right, visit
//		if (this->m_pLeftChild) this->m_pLeftChild->parse(parser, mode, maxdepth - 1);
//		if (this->m_pRightChild) this->m_pRightChild->parse(parser, mode, maxdepth - 1);
//		if (this->HasNode()) parser->ParseNode(this);
//		break;
//	}
//
//	parser->pop();
//}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/// @todo folyt kov http://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/
int BinaryTree::PreorderIterator::IsDone()
{
	return m_sNode.empty() && m_pCurrent == nullptr;
}

void BinaryTree::PreorderIterator::Next()
{
	BinaryTree *node = (BinaryTree *)m_pCurrent;
	if (node != nullptr) {
		if (m_parser)
			m_parser->ParseNode(node);

		if (node->HasRight()) {
			// push, right
			if (m_parser)
				m_parser->Push(node);
			m_sNode.push(node);

			node = node->GetRightChild();
		}

		if (m_parser)
			m_parser->Push(node);

		// itt nem pusholunk a stackbe, csak a parsert hivjuk meg
		node = node->GetLeftChild();
	}
	else {
		// pop 
		if (m_parser)
			m_parser->Pop(node);
		node = (BinaryTree *)m_sNode.top();
		m_sNode.pop();
	}

	// keep going 
	m_pCurrent = node;
}

int BinaryTree::PreorderIterator::HasNext()
{
	BinaryTree * node = (BinaryTree *)m_pCurrent;
	return node && (node->HasLeft() || node->HasRight());
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ====================================================================================================================================================================================
// Implementation of ChainTree
// ====================================================================================================================================================================================

void ChainTree::SetParent(ChainTree *&parent){
	if (parent)
		parent->m_pChild = this;
	this->m_pParentNode = parent;
}	

void ChainTree::SetChild(ChainTree *&child){
	if (child)
		child->m_pParentNode = this;
	this->m_pChild = child;
}	

void ChainTree::SetLeft(ChainTree *&left){
	if (left)
		left->m_pRight = this;
	this->m_pLeft = left;
}

void ChainTree::SetRight(ChainTree *&right){
	if (right)
		right->m_pLeft = this;
	this->m_pRight = right;
}

// -------- 

/// @todo nem szur be rendesen
ChainTree* ChainTree::InsertChild(ChainTree *child){
	ChainTree *newchild = child;
	if (newchild== nullptr) {
		newchild = new ChainTree(this);
	}

	this->m_pChild = newchild;
	this->m_pFarRightChild->InsertRight(newchild);
	this->m_pFarRightChild = newchild;
	
	// is it was the first element
	if (this->m_pFarLeftChild== nullptr)
		this->m_pFarLeftChild = newchild;

	return newchild;
}

Iterator * ChainTree::GetIterator(TRAVELSAL_Gen_Type mode)
{
	return nullptr;
}

ChainTree* ChainTree::InsertNeighbour(ChainTree* _newNode, ChainTree*& insSide, ChainTree*&parentSide)
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

inline ChainTree* ChainTree::InsertLeft(ChainTree *left) {
	return InsertNeighbour(left, m_pLeft, this->GetParent()->m_pFarLeftChild);
}

inline ChainTree* ChainTree::InsertRight(ChainTree *right){
	return InsertNeighbour(right, m_pRight, this->GetParent()->m_pFarRightChild);
}

//void ChainTree::parse(TreeParser* parser, enum NP_searchMode_e mode, int maxdepth) {
//	if (!parser) return;
//	if (maxdepth < 0) return;
//
//	///@ todo ezeket a bejaro fuggvenyeket ki kene venni inkabb kulon metodusokba
//	///@todo feltelezezzuk, hogy nincs balra szomszed -> parseolas elott rendezni kell a fat, hogy a kozvetlen leszarmazott legyen a szelso-baloldali elem
//	// innentol csak jobbra indulunk
//
//	if (mode == NP_search_inorder) {
//		parser->push();
//		if (this->HasNode()) parser->ParseNode(this);
//		if (this->hasChild()) m_pChild->parse(parser, mode, maxdepth - 1);
//
//		parser->pop();
//
//		if (this->hasRight()) m_pRight->parse(parser, mode, maxdepth - 1);
//	}
//	else {
//		parser->push();
//		if (this->hasChild()) m_pChild->parse(parser, mode, maxdepth - 1);
//		if (this->HasNode()) parser->ParseNode(this);
//
//		parser->pop();
//
//		if (this->hasRight()) m_pRight->parse(parser, mode, maxdepth - 1);
//	}
//}

// ====================================================================================================================================================================================
// Implementation of ListTree
// ====================================================================================================================================================================================

void ListTree::SetParent(ListTree *parent, int autoAddParent)
{
	this->m_pParentNode = parent;
}

void ListTree::InsertChild(ListTree *child)
{
	this->m_vChildren.push_back(child);
	child->SetParent(this, 0);
}

Iterator * ListTree::GetIterator(TRAVELSAL_Gen_Type mode)
{
	return nullptr;
}

//void ListTree::parse(TreeParser* parser, enum NP_searchMode_e mode, int maxdepth) {
//	if(!parser) return;
//	if(maxdepth<0) return;
//
//	///@todo fa bejarasi strategiak
//
//	parser->push();
//
//	///@ todo ezeket a bejaro fuggvenyeket ki kene venni inkabb kulon metodusokba
//
//	//	inorder:= parse, majd gyerek, kulonben forditva
//	if (mode == NP_search_inorder) {
//		// kurrens node feldolg
//
//		if (this->HasNode())
//			parser->ParseNode(this);
//
//		// kozvetlen leszarmazott
//		if (this->hasChild()) {
//			for (int i = 0; i < this->getChildCount(); i++) {
//				this->m_vChildren[i]->parse(parser, mode, maxdepth - 1);
//			}
//		}
//	}
//	else
//	{
//		
//		if (this->hasChild()) {
//			for (int i = 0; i < this->getChildCount(); i++) {
//				this->m_vChildren[i]->parse(parser, mode, maxdepth - 1);
//			}
//		}
//
//		if (this->HasNode())
//			parser->ParseNode(this);
//	}
//
//	parser->pop();
//}


