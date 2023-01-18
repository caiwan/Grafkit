#include <memory>
#include "gtest/gtest.h"
#include "utils/node.h"

using std::shared_ptr;

class NodeTest : public testing::Test {
public:
	Node* node1;
	Node* node2;
	Node* node3;
	Node* node4;

public:
	void SetUp() {
		this->node1 = new Node;
		this->node2 = new Node;
		this->node3 = new Node;
		this->node4 = new Node;

		node1->insertNodeAfter(node2);
		node2->insertNodeAfter(node3);
	}

	void TearDown(){
		delete node1;
		delete node2;
		delete node3;
		delete node4;
	}

};

TEST_F(NodeTest, InsertNext) {
	ASSERT_EQ(node2, node1->getNextNode());
	ASSERT_EQ(node3, node2->getNextNode());
	ASSERT_EQ(nullptr, node3->getNextNode());
		   
	ASSERT_EQ(nullptr, node1->getPrevNode());
	ASSERT_EQ(node1, node2->getPrevNode());
	ASSERT_EQ(node2, node3->getPrevNode());

	// insert inbetween
	node2->insertNodeAfter(node4);
	
	ASSERT_EQ(node4, node2->getNextNode()  );
	ASSERT_EQ(node3  , node4->getNextNode());

	ASSERT_EQ(node2  , node4->getPrevNode());
	ASSERT_EQ(node4, node3->getPrevNode()  );
}

TEST_F(NodeTest, InsertPrev) {
	ASSERT_EQ(node2,   node1->getNextNode());
	ASSERT_EQ(node3,   node2->getNextNode());
	ASSERT_EQ(nullptr, node3->getNextNode());
		   								   
	ASSERT_EQ(nullptr, node1->getPrevNode());
	ASSERT_EQ(node1  , node2->getPrevNode());
	ASSERT_EQ(node2  , node3->getPrevNode());

	// insert inbetween
	node2->insertNodeBefore(node4);

	ASSERT_TRUE(node4, node2->getPrevNode());
	ASSERT_TRUE(node2  , node4->getNextNode());

	ASSERT_TRUE(node1  , node4->getPrevNode());
	ASSERT_TRUE(node4, node1->getNextNode()  );
}


TEST_F(NodeTest, Delete) {
	node2->removeNode();

	ASSERT_EQ(nullptr, node2->getNextNode());
	ASSERT_EQ(nullptr, node2->getPrevNode());

	ASSERT_EQ(node3,   node1->getNextNode());
	ASSERT_EQ(nullptr, node3->getNextNode());

	ASSERT_EQ(nullptr, node1->getPrevNode());
	ASSERT_EQ(node1,   node3->getPrevNode());
}

TEST_F(NodeTest, Iterate) {
	NodeIterator * it = dynamic_cast<NodeIterator *>(node2->getIterator());
	ASSERT_NE(nullptr, it);

	it->first();
	ASSERT_EQ(node1, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_TRUE(it->hasNext());

	it->next();
	ASSERT_EQ(node2, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_TRUE(it->hasNext());

	it->next();
	ASSERT_EQ(node3, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_FALSE(it->hasNext());

	it->next();
	ASSERT_TRUE(it->isDone());
	ASSERT_TRUE(it->hasNext());
	
	// itt meg kell nezni, hogy nem szalad-e ra null-ra
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_NE(node3, it->getCurrent());
	ASSERT_NE(node2, it->getCurrent());
	ASSERT_NE(node1, it->getCurrent());
}

/**
	Test that the end of the chain trees are liked together at their ends
*/
TEST_F(NodeTest, IterateCycle) {
	NodeIterator * it = dynamic_cast<NodeIterator *>(node2->getIterator());
	ASSERT_NE(nullptr, it);

	it->first(); ASSERT_NE(nullptr, it->getCurrent());
	it->next();  ASSERT_NE(nullptr, it->getCurrent());
	it->next();  ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_TRUE(it->getCurrent() == node3);

	// we've reached the end of the chain, the crsr point at the zero/null object
	// the iterator has a next element at this point s well; the first one 
	it->next();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_TRUE(it->isDone());
	ASSERT_TRUE(it->hasNext());

	// at the next iteration the pointer should point at the first element as well.
	it->next();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node1, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_TRUE(it->hasNext());

	it->next();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node2, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_TRUE(it->hasNext());

	it->next();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node3, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_FALSE(it->hasNext());

	// and we've reached the end as well
	it->next();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_TRUE(it->isDone());
	ASSERT_TRUE(it->hasNext());
}

// Backward iteration
TEST_F(NodeTest, IterateBackwards) {
	NodeIterator * it = dynamic_cast<NodeIterator *>(node2->getIterator());
	ASSERT_NE(nullptr, it);

	it->last();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node3, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_TRUE(it->hasPrev());

	it->previous();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node2, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_TRUE(it->hasPrev());

	it->previous();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node1, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_FALSE(it->hasPrev());

	it->previous();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_TRUE(it->isDone());
	ASSERT_TRUE(it->hasPrev());
}

///@ todod test for zero object 

/**
Test that the end of the chain trees are liked together at their ends
*/
TEST_F(NodeTest, IterateBackwardsCycle) {
	NodeIterator * it = dynamic_cast<NodeIterator *>(node2->getIterator());
	ASSERT_NE(nullptr, it);

	it->last(); ASSERT_NE(nullptr, it->getCurrent());
	it->previous(); ASSERT_NE(nullptr, it->getCurrent());
	it->previous(); ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node1, it->getCurrent());

	it->previous();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_TRUE(it->isDone());
	ASSERT_TRUE(it->hasPrev());

	it->previous();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node3, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_TRUE(it->hasPrev());

	it->previous();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node2, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_TRUE(it->hasPrev());

	it->previous();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_EQ(node1, it->getCurrent());
	ASSERT_FALSE(it->isDone());
	ASSERT_FALSE(it->hasPrev());

	it->previous();
	ASSERT_NE(nullptr, it->getCurrent());
	ASSERT_TRUE(it->isDone());
	ASSERT_TRUE(it->hasPrev());
}
