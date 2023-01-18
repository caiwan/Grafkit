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
	//class IRenderAssetRepository;

	typedef Ref<IResource> IResourceRef;

	///@todo ezt ki kell pucolni teljesen

	/**
	A base class for collectable render assets for bulk loading
	*/

	// ez nem lehet aligned, hanem mindenkinak aki hasznalja
	//__declspec(align(16)) class IResource : virtual public Referencable, public AlignedNew<IResource>
	class IResource : virtual public Referencable
	{
		friend class IResourceManager;
		friend class IRenderAssetRepository;
		friend class IRenderAssetGenerator;

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

		virtual const char* GetBucketID() = 0;

	protected:
		/// Sets asset manager 
		std::string m_name;

	};
}