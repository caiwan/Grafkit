#include "../stdafx.h"

#include "node.h"

#include "exceptions.h"
using namespace FWdebugExceptions;

Node::Node() : 
	m_pObject(nullptr),
	m_rNodeNext(nullptr),
	m_rNodePrev(nullptr)
{
}

Node::~Node()
{
}

void Node::InsertNodeBefore(Node * node)
{
	node->m_rNodePrev = m_rNodePrev;

	if (m_rNodePrev!= nullptr)
		m_rNodePrev->m_rNodeNext = node;

	m_rNodePrev = node;
	node->m_rNodeNext = this;
}

void Node::InsertNodeAfter(Node * node)
{
	node->m_rNodeNext = this->m_rNodeNext;

	if (m_rNodeNext!= nullptr)
		this->m_rNodeNext->m_rNodePrev = node;

	m_rNodeNext = node;
	node->m_rNodePrev = this;
}

Node* Node::RemoveNode()
{
	m_rNodePrev->m_rNodeNext = m_rNodeNext;
	m_rNodeNext->m_rNodePrev = m_rNodePrev;

	m_rNodeNext = nullptr;
	m_rNodePrev = nullptr;

	return this;
}

Iterator * Node::GetIterator()
{
	return new NodeIterator(this);
}

// ================================================================================

NodeIterator::NodeIterator() : 
	m_zero(new Node)
{
	m_current = m_zero;
}

NodeIterator::NodeIterator(Node* start, int isBreakChain) : NodeIterator()
{
	// to keep it cycable
	Node *&head = m_zero->m_rNodeNext;
	Node *&foot = m_zero->m_rNodePrev;

	head = start;
	foot = start;
	if (!isBreakChain)
	{
		// find very first item and link it
		while (head->m_rNodePrev!= nullptr){
			head = head->GetPrevNode();
		} 
		head->m_rNodePrev = m_zero;
	
		// find very last item
		while (foot->m_rNodeNext!= nullptr) {
			foot = foot->GetNextNode();
		}

		foot->m_rNodeNext = m_zero;
	} else 
	{
		// threat current node as the very first item
		// and break chain right there, ignoring the previous ones
		
		/// @todo implement and test it 
		throw EX_DETAILS(NotImplementedMethodException, "Breaking chain has not implemented yet");
	}
}

NodeIterator::~NodeIterator()
{
	Node *foot = m_zero->m_rNodePrev;
	Node *head = m_zero->m_rNodeNext;

	delete m_zero;
}

void NodeIterator::First()
{
	this->m_current = this->m_zero->GetNextNode();
}

void NodeIterator::Last()
{
	this->m_current = this->m_zero->GetPrevNode();
}

int NodeIterator::IsDone()
{
	return m_current == m_zero;
}

void NodeIterator::Next()
{
	m_current = m_current->m_rNodeNext;
}

void NodeIterator::Previous()
{
	m_current = m_current->m_rNodePrev;
}

int NodeIterator::HasNext()
{
	return m_current->m_rNodeNext != m_zero;
}

int NodeIterator::HasPrev()
{
	return m_current->m_rNodePrev != m_zero;
}

void NodeIterator::DeleteChain()
{
	for (First(); IsDone(); Next())
		delete GetCurrent();
}
