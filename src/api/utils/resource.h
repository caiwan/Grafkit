/**
A generator interface for assets 
*/

#pragma once

#include <map>
#include <string>

#include "../utils/persistence/persistence.h"

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
	protected:

		void _serialize(Archive& ar);

	public:
		IResource();
		virtual ~IResource();

		std::string GetName() { return this->m_name; }
		void SetName(std::string name) { m_name = name; }

	protected:
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
