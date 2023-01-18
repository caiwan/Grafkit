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

void Node::insertNodeBefore(Node * node)
{
	node->m_rNodePrev = m_rNodePrev;

	if (m_rNodePrev!= nullptr)
		m_rNodePrev->m_rNodeNext = node;

	m_rNodePrev = node;
	node->m_rNodeNext = this;
}

void Node::insertNodeAfter(Node * node)
{
	node->m_rNodeNext = this->m_rNodeNext;

	if (m_rNodeNext!= nullptr)
		this->m_rNodeNext->m_rNodePrev = node;

	m_rNodeNext = node;
	node->m_rNodePrev = this;
}

Node* Node::removeNode()
{
	m_rNodePrev->m_rNodeNext = m_rNodeNext;
	m_rNodeNext->m_rNodePrev = m_rNodePrev;

	m_rNodeNext = nullptr;
	m_rNodePrev = nullptr;

	return this;
}

Iterator * Node::getIterator()
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
			head = head->getPrevNode();
		} 
		head->m_rNodePrev = m_zero;
	
		// find very last item
		while (foot->m_rNodeNext!= nullptr) {
			foot = foot->getNextNode();
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

void NodeIterator::first()
{
	this->m_current = this->m_zero->getNextNode();
}

void NodeIterator::last()
{
	this->m_current = this->m_zero->getPrevNode();
}

int NodeIterator::isDone()
{
	return m_current == m_zero;
}

void NodeIterator::next()
{
	m_current = m_current->m_rNodeNext;
}

void NodeIterator::previous()
{
	m_current = m_current->m_rNodePrev;
}

int NodeIterator::hasNext()
{
	return m_current->m_rNodeNext != m_zero;
}

int NodeIterator::hasPrev()
{
	return m_current->m_rNodePrev != m_zero;
}

void NodeIterator::deleteChain()
{
	for (first(); isDone(); next())
		delete getCurrent();
}
