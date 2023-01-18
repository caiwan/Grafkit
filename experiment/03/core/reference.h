#pragma once

/// @file taken from libosmscout: https://github.com/nkostelnik/libosmscout/blob/master/libosmscout/include/osmscout/util/Reference.h

// Turn off assertion. Will lead to unchecked nullpointer access
#define _NOASSERT

#ifndef _NOASSERT
#include "../debug/test.h"
#else 
#define DEBUG_ASSERT(x)
#endif

/**
	Baseclass for all classes that support reference counting.
 */
class Referencable {
public:
	Referencable() : _ref_count(0) { }

	/**
	 Add a reference to this object.
	 Increments the internal reference counter.

	 **refracted** to have correspondence to [IUnknown::AddRef](https://msdn.microsoft.com/en-us/library/windows/desktop/ms691379(v=vs.85).aspx)
	*/
	inline size_t AddRef() {
		++_ref_count;
		return _ref_count;
	}

	/**
	 Remove a reference from this object.

	 Decrements the internal reference counter.

	 **refracted** to have correspondence to [IUnknown::Release](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682317(v=vs.85).aspx)
	*/
	inline size_t Release() { _ref_count--; return _ref_count; }

	/**
		Returns the current reference count.
	*/
	inline const size_t GetReferenceCount() const { return _ref_count; }

private:
	size_t _ref_count;
};

/**
LazyRef does delay the allocation of the referenced object of type T. It also
implements references counting on copy and assignment, allow to exchange
costly copy operations with cheaper reference assignment operations. Using LazyRef
is useful if you reference objects at multiple locations where the object is only
destroyed if all locations delete the object and where copying the object is expensive.

LazyRef allocates a new object instance the first time the reference is dereferenced and
the object instance of type T is accessed.

Note that type T must inherit from class Referencable!
*/
template <typename T> class LazyRef {
private:
	mutable T* ptr;

public:
	/** Default constructor. Creates an empty reference. */
	inline LazyRef() : ptr(NULL) { }

	/** Creates an reference holding an instance of T. */
	LazyRef(T* pointer) : ptr(pointer) {
		if (ptr != NULL) {
			ptr->AddRef();
		}
	}

	/**
	 Copy constructor for the same type of reference.
	*/
	inline LazyRef(const LazyRef<T>& other)
		: ptr(other.ptr)
	{
		if (ptr != NULL) {
			ptr->AddRef();
		}
	}

	/**
	 Destructor
	*/
	inline ~LazyRef()
	{
		if (ptr != NULL &&
			ptr->Release() == 0) {
			delete ptr;
		}
	}

	/**
	 Assignment operator.

	 Assigns a new value to the reference. The reference count of the
	 object hold by the handed reference - if the pointer is not NULL -
	 will be incremented. The reference count of the old value will be
	 decremented and freed, if the count reached 0.
	*/
	void operator=(const LazyRef<T>& other)
	{
		if (ptr != other.ptr) {
			if (ptr != NULL &&
				ptr->Release() == 0) {
				delete ptr;
			}

			ptr = other.ptr;

			if (ptr != NULL) {
				ptr->AddRef();
			}
		}
	}

	/**
	 arrow operator.

	 Returns the underlying pointer. Makes the reference behave like a pointer.
	*/
	T* operator->() const
	{
		if (ptr == NULL) {
			ptr = new T();
			ptr->AddRef();
		}

		return ptr;
	}

	/**
		Dereference operator.

		Returns a reference to the underlying object. Makes the reference behave like a pointer.
	*/
	T& operator*() const
	{
		DEBUG_ASSERT(ptr == NULL);
		return *ptr;
	}
};




/**
Ref handles references to object using reference counting semantic. The object of type
T is onbly deleted if all references have gone invalid.

Note that type T must inherit from class Referencable!
*/
template <typename T> class Ref
{
public:
	/**
	 Default constructor. Creates an empty reference.
	*/
	inline Ref() : ptr(NULL) {}

	/**
	 Creates an reference holding an instance of T.
	*/
	inline Ref(T* pointer) : ptr(pointer) {
		if (ptr != NULL) {
			ptr->AddRef();
		}
	}

	/**
	 Copy constructor
	*/
	inline Ref(const Ref<T>& other) : ptr(other.ptr) {
		if (ptr != NULL) {
			ptr->AddRef();
		}
	}

	/**
	  Access operator.

	  Returns the underlying pointer. Note that the object is still
	  hold by the Reference.
	 */
	inline T* Get() const {
		return ptr;
	}

	/**
	 Copy constructor
	*/
	template<typename T1>
	inline Ref(const Ref<T1>& other)
		: ptr(other.Get())
	{
		if (ptr != NULL) {
			ptr->AddRef();
		}
	}

	/**
	 Destructor
	*/
	inline ~Ref()
	{
		if (ptr != NULL &&
			ptr->Release() == 0) {
			delete ptr;
		}
	}

	/**
	Assignment without using `operator =`.

	Assigns a new value to the reference. The reference count of the
	new object - if the pointer is not NULL - will be incremented.
	The reference count of the old value will be decremented and freed,
	if the count reached 0.
	*/

	void AssingnRef(T* pointer)
	{
		if (ptr != pointer) {

			if (pointer != NULL) {
				pointer->AddRef();
			}

			if (ptr != NULL &&
				ptr->Release() == 0) {
				delete ptr;
			}

			ptr = pointer;
		}
	}

	/**
	 Assignment operator.

	 Assigns a new value to the reference. The reference count of the
	 new object - if the pointer is not NULL - will be incremented.
	 The reference count of the old value will be decremented and freed,
	 if the count reached 0.
	*/
	Ref<T>& operator=(T* pointer)
	{
		AssingnRef(pointer);
		return *this;
	}

	/**
	 Assignment operator.

	 Assigns a new value to the reference. The reference count of the
	 object hold by the handed reference - if the pointer is not NULL -
	 will be incremented. The reference count of the old value will be
	 decremented and freed, if the count reached 0.
	*/
	Ref<T>& operator=(const Ref<T>& other)
	{
		if (&other != this && this->ptr != other.ptr) {
			if (ptr != NULL &&
				ptr->Release() == 0) {
				delete ptr;
			}

			ptr = other.ptr;

			if (ptr != NULL) {
				ptr->AddRef();
			}
		}

		return *this;
	}

	template<typename T1> Ref<T>& operator=(const Ref<T1>& other)
	{
		if (&other != this && this->ptr != other.Get()) {
			if (ptr != NULL &&
				ptr->Release()) {
				delete ptr;
			}

			ptr = other.Get();

			if (ptr != NULL) {
				ptr->AddRef();
			}
		}

		return *this;
	}


	inline bool Valid() const { return ptr != NULL; }
	inline bool Invalid() const { return ptr == NULL; }

	/**
	   arrow operator.

	   Returns the underlying pointer. Makes the reference behave like a pointer.
	*/
	T* operator->() const {
		DEBUG_ASSERT(Valid()); // Method calling on NULL pointer is forbidden
		return ptr;
	}

	/**
	 Dereference operator.

	 Returns a reference to the underlying object. Makes the reference behave
	 like a pointer.
	*/
	T& operator*() const {
		DEBUG_ASSERT(Valid());
		return *ptr;
	}

	/**
	 Type conversion operator.

	 Returns the underlying pointer. Allows reference to be
	 passed as a parameter where the base pointer type is required.
	*/
	operator T*() const {
		return ptr;
	}

	/**
	 Type conversion operator.

	 Returns the underlying object as reference. Allows reference to be
	 passed as a parameter where the object type is required.
	*/
	operator T&() const {
		return *ptr;
	}

	bool operator==(const Ref<T>& other) const {
		return ptr == other.ptr;
	}

	bool operator!=(const Ref<T>& other) const {
		return ptr != other.ptr;
	}

	bool operator<(const Ref<T>& other) const {
		return ptr < other.ptr;
	}

private:
	T* ptr;
};
