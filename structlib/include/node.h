/**
This file has a node interface for every node type class for containers
*/

#pragma once 

#include "iterator.h"

class Referencable;
class NodeIterator;
class Node;

class Node : /*virtual public Referencable,*/ virtual public Iterable{
	friend class NodeIterator;
	public:
		Node();
		virtual ~Node();

		void insertNodeBefore(Node* node);
		void insertNodeAfter(Node* node);
	
		Node* getPrevNode() { return this->m_rNodePrev;}
		Node* getNextNode() { return this->m_rNodeNext; }

		Node* removeNode();
		void setObject(Referencable* pObject = nullptr);

		Referencable *& getObject() { return m_pObject; }
		int hasNode() { return m_pObject != nullptr; }

		Iterator* getIterator();

	protected:
		Node* m_rNodePrev;
		Node* m_rNodeNext;

		Referencable* m_pObject;
};



class NodeIterator : public Iterator
{
public:
	NodeIterator();
	NodeIterator(Node* start, int isBreakChain = false);
	~NodeIterator();

	void first();
	void last();
	int isDone();

	void next();
	void previous();
	
	int hasNext();
	int hasPrev();

	Node* getCurrent() { return m_current; }

	// todo ... 
	Node* getFirst() { return m_zero->m_rNodeNext; }
	Node* getLast() { return m_zero->m_rNodePrev; }

	void deleteChain();

private:
	Node* m_current;
	Node* m_zero;
};
