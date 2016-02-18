/**
Unittests for binary trees 
*/

#include "gtest/gtest.h"

#include "tree.h"

class NodeRef : virtual public Referencable {
};

class BinaryTreeTest : public testing::Test{
public:
	NodeRef* nodeA;
	NodeRef* nodeB;
	NodeRef* nodeC;	
	NodeRef* nodeD;
	NodeRef* nodeE;
	NodeRef* nodeF;

	BinaryTree * tree;

public:
	void SetUp() {
		nodeA = new NodeRef;
		nodeB = new NodeRef;
		nodeC = new NodeRef;
		nodeD = new NodeRef;
		nodeE = new NodeRef;
		nodeF = new NodeRef;

		tree = new BinaryTree();

		tree->setObject(nodeA);
		tree->insertLeftChild()->setObject(nodeB);
		tree->insertRightChild()->setObject(nodeC);
	}

	void TearDown() {
		delete tree;

		delete nodeA;
		delete nodeB;
		delete nodeC;
		delete nodeD;
		delete nodeE;
		delete nodeF;
	}

};


TEST_F(BinaryTreeTest, InsertChild) {
	ASSERT_EQ(nullptr, tree->getParent());
	ASSERT_EQ(nodeA, tree->getObject());

	ASSERT_TRUE(tree->hasLeft());
	ASSERT_EQ(nodeB, tree->getLeftChild()->getObject());
	ASSERT_EQ(nodeA, tree->getLeftChild()->getParent()->getObject());

	ASSERT_TRUE(tree->hasRight());
	ASSERT_EQ(nodeC, tree->getRightChild()->getObject());
	ASSERT_EQ(nodeA, tree->getRightChild()->getParent()->getObject());
}

TEST_F(BinaryTreeTest, DeleteChild) {
	FAIL(); // not supported yet~

	//tree->deleteLeftChild();
	//ASSERT_FALSE(tree->hasLeft());
	//ASSERT_FALSE(tree->getLeftChild()== nullptr);
	//
	//ASSERT_TRUE(tree->hasRight());
	//ASSERT_TRUE(tree->getRightChild()->getObject() == nodeC);

	//tree->deleteRightChild();

	//ASSERT_FALSE(tree->hasRight());
	//ASSERT_FALSE(tree->getRightChild()->getObject() == nodeC);
}


TEST_F(BinaryTreeTest, DetachChild) {
	
	// what ?
	
	BinaryTree* subtreeC = tree->getRightChild(); 
	subtreeC->insertLeftChild()->setObject(nodeD);

	ASSERT_TRUE(subtreeC->getObject() == nodeC);
	ASSERT_TRUE(subtreeC->getParent()->getObject() == nodeA);

	// delete?

	subtreeC = subtreeC->detachLeftChild();

	ASSERT_TRUE(subtreeC->getParent()== nullptr);
	ASSERT_TRUE(subtreeC->getObject() == nodeC);
	ASSERT_TRUE(subtreeC->getLeftChild()->getObject() == nodeD);

	// delete?
}

