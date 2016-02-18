/**
Iterator + iterable interface 
*/

#pragma once

#include "reference.h"

/// @todo docs

class Node;

class Iterator;
class Iterable;

/**
An iterator interface
Red book, pg. 25
*/
class Iterator
{
	friend class Iterable;
public:
	Iterator() {}
	virtual ~Iterator() {}

	virtual void first() = 0;
	virtual void last() = 0;
	virtual int isDone() = 0;	

	virtual void next() = 0;
	virtual void previous() = 0;
	

	virtual int hasNext() = 0;
	virtual int hasPrev() = 0;
	
	///@todo operator ++,-- and () for isDone

	virtual Node* getCurrent() = 0;
};

/// Iterable interface that generates an iterator for a certain class
class Iterable {
	
public:
	virtual ~Iterable() {}
	virtual Iterator* getIterator() = 0;
};

// =================================================================================
