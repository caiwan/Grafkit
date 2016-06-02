/**
A generator interface for assets 
*/

#pragma once

#include <map>
#include <string>

//#include "memory_align.h"

// egyelore nem szerencsetlenkedunk a guid-dal, de ha megirtam eddig, akkor maradjon
#define _NO_UUID_

#ifndef _NO_UUID_
#include "guid.h"
#endif 

#include "reference.h"
#include "exceptions.h"

namespace Grafkit{

	class IResource;
	class IResourceManager;
	class IResourceBuilder;

	///@todo ezt ki kell pucolni teljesen

	/**
	A base class for collectable render assets for bulk loading and reloading 
	*/
	class IResource : virtual public Referencable
	{
		friend class IResourceManager;

	public:
		IResource();
		virtual ~IResource();

		std::string GetName() { return this->m_name; }
		void SetName(std::string name);

#ifndef _NO_UUID_
		Guid GetUUID() { return this->m_guid; }
		Guid GenerateUUID();
		void SetUUID(Guid uuid) { this->m_guid = uuid; }
	protected:
		Guid m_guid;
#endif // _NO_UUID_

		// erre valami mas megoldst kellene talalni majd 
		// virtual const char* GetBucketID() = 0;

	protected:
		/// Sets asset manager 
		std::string m_name;

	};

	/**
	Ebbe csomagoljuk bele a resourceokat, amiket elteszunk; lenyeg, hogy ezzel barmibol 
	lehet majd resouce-t generalni, ha kell 
	*/
	template <typename T> class Resource : public IResource, public Ref<T>
	{
	public:
		Resource() : IResource(), Ref<T>() {}

		Resource(Resource* ptr) : IResource(), Ref<T>(ptr) {}
		Resource(Ref<Resource> ref) : IResource(), Ref<T>(ref) {}

		Resource(Ref<T> tref) : IResource(), Ref<T>(tref) {}
		Resource(T* tptr) : IResource(), Ref<T>(tptr) {}

		operator Ref<T> () { return Ref<T>(dynamic_cast<T*>(this->Get())); }
		operator T * const & () { return dynamic_cast<T*>(this->Get()); }
		operator T& () { return *(this->Get()); }

	};

}

