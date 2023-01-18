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
	ASSERT_EQ(nodeA, tree->getObject<NodeRef>());

	ASSERT_TRUE(tree->hasLeft());
	ASSERT_EQ(nodeB, tree->getLeftChild()->getObject<NodeRef>());
	ASSERT_EQ(nodeA, tree->getLeftChild()->getParent()->getObject<NodeRef>());

	ASSERT_TRUE(tree->hasRight());
	ASSERT_EQ(nodeC, tree->getRightChild()->getObject<NodeRef>());
	ASSERT_EQ(nodeA, tree->getRightChild()->getParent()->getObject<NodeRef>());
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

	ASSERT_TRUE(subtreeC->getObject<NodeRef>() == nodeC);
	ASSERT_TRUE(subtreeC->getParent()->getObject<NodeRef>() == nodeA);

	// delete?

	subtreeC = subtreeC->detachLeftChild();

	ASSERT_TRUE(subtreeC->getParent()== nullptr);
	ASSERT_TRUE(subtreeC->getObject<NodeRef>() == nodeC);
	ASSERT_TRUE(subtreeC->getLeftChild()->getObject<NodeRef>() == nodeD);

	// delete?
}

// === TREE TRAVERSAL, Recursive ===
TEST_F(BinaryTreeTest, RecursiveTravelsal_Inorder) { FAIL(); }
TEST_F(BinaryTreeTest, RecursiveTravelsal_Preorder) { FAIL(); }
TEST_F(BinaryTreeTest, RecursiveTravelsal_Postorder) { FAIL(); }

// === TREE TRAVERSAL, Iterative ===
TEST_F(BinaryTreeTest, IterativeTravelsal_Inorder) {
	/* A B C */
	Iterator* iterator = tree->getIterator();
	ASSERT_NE(iterator, nullptr);

	iterator->first();
	ASSERT_FALSE(iterator->isDone());
	ASSERT_TRUE(iterator->hasNext());
	ASSERT_EQ(iterator->getCurrent()->getObject<NodeRef>(), nodeA);
	
	iterator->next();
	ASSERT_FALSE(iterator->isDone());
	ASSERT_TRUE(iterator->hasNext());
	ASSERT_EQ(iterator->getCurrent()->getObject<NodeRef>(), nodeB);
	
	iterator->next();
	ASSERT_FALSE(iterator->isDone());
	ASSERT_TRUE(iterator->hasNext());
	ASSERT_EQ(iterator->getCurrent()->getObject<NodeRef>(), nodeC);

	ASSERT_TRUE(iterator->isDone());
	ASSERT_FALSE(iterator->hasNext());
}

TEST_F(BinaryTreeTest, IterativeTravelsal_Preorder) {
	FAIL();
}

TEST_F(BinaryTreeTest, IterativeTravelsal_Postorder) {
	FAIL();
}