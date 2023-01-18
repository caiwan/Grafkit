/**
This file has a node interface for every node type class for containers
*/

#pragma once 

#include "iterator.h"

class Referencable;
class NodeIterator;
class Node;

class Node : virtual public Iterable{
	friend class NodeIterator;
	public:
		Node();
		virtual ~Node();

		void insertNodeBefore(Node* node);
		void insertNodeAfter(Node* node);
	
		Node* getPrevNode() { return this->m_rNodePrev;}
		Node* getNextNode() { return this->m_rNodeNext; }

		Node* removeNode();
		
		template<typename T> T* getObjectDynamic() { return dynamic_cast<T*>(m_pObject); }
		template<typename T> T* getObjectStatic() { return static_cast<T*>(m_pObject); }
		template<typename T> T* getObject() { return (T*)m_pObject; }

		template<typename T> void setObject(T* obj) { m_pObject = obj; }
		void setObject(void* obj) {m_pObject = obj; }

		int hasNode() { return m_pObject != nullptr; }

		Iterator* getIterator();

	protected:
		Node* m_rNodePrev;
		Node* m_rNodeNext;

		void *m_pObject;
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
 