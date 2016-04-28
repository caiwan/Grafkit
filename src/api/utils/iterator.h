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

	virtual void First() = 0;
	virtual void Last() = 0;
	virtual int IsDone() = 0;	

	virtual void Next() = 0;
	virtual void Previous() = 0;
	

	virtual int HasNext() = 0;
	virtual int HasPrev() = 0;
	
	///@todo operator ++,-- and (bool) for isDone

	virtual Node* GetCurrent() = 0;
};

/// Iterable interface that generates an iterator for a certain class
class Iterable {
	
public:
	virtual ~Iterable() {}
	virtual Iterator* GetIterator() = 0;
};

// =================================================================================
