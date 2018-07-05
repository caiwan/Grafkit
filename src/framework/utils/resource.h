/**
A generator interface for assets
*/

#pragma once

#include <string>

#include "reference.h"

namespace Grafkit {

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

	public:
		std::string GetName() { return this->m_name; }
		void SetName(std::string name) { m_name = name; }

		virtual void* GetRaw() = 0;

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

		operator Ref<T>() { return Ref<T>(dynamic_cast<T*>(this->Get())); }
		operator T * const & () { return dynamic_cast<T*>(this->Get()); }
		operator T& () { return *(this->Get()); }

	    void* GetRaw() override { return this->Get(); }

	};

}
