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

		void InsertNodeBefore(Node* node);
		void InsertNodeAfter(Node* node);
	
		Node* GetPrevNode() { return this->m_rNodePrev;}
		Node* GetNextNode() { return this->m_rNodeNext; }

		Node* RemoveNode();
		
		template<typename T> T* GetDynamic() { return dynamic_cast<T*>(m_pObject); }
		template<typename T> T* GetStatic() { return static_cast<T*>(m_pObject); }
		template<typename T> T* Get() { return (T*)m_pObject; }

		template<typename T> void Set(T* obj) { m_pObject = obj; }
		void Set(void* obj) {m_pObject = obj; }

		int HasNode() { return m_pObject != nullptr; }

		Iterator* GetIterator();

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

	void First();
	void Last();
	int IsDone();

	void Next();
	void Previous();
	
	int HasNext();
	int HasPrev();

	Node* GetCurrent() { return m_current; }

	// todo ... 
	Node* GetFirst() { return m_zero->m_rNodeNext; }
	Node* GetLast() { return m_zero->m_rNodePrev; }

	void DeleteChain();

private:
	Node* m_current;
	Node* m_zero;
};
 