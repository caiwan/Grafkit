#pragma once

#include <string>

#include "exceptions.h"

#include "reference.h"
#include "guid.h"

namespace FWassets {

	class IResource;
	typedef Ref<IResource> IResourceRef;

	/// an interface for resources
	class IResource : public Referencable
	{
	public:
		IResource() {}
		virtual ~IResource() {}

		virtual void* GetData() = 0;
		virtual size_t GetSize() = 0;
	};

	/// An interface for resource obtaining mechnaism
	class IResourceFactory 
	{
	protected:
		IResourceFactory() {}
		virtual ~IResourceFactory() {}

	public:
		virtual IResourceRef GetResourceByName(std::string name) = 0;
		virtual IResourceRef GetResourceByUUID(Guid uuid) = 0;

	};

	// ==============================================================================

	// loads resources from file
	class FileResourceManager : IResourceFactory
	{
	public:
		class FileResource;

	public:
		FileResourceManager(const char* root);
		FileResourceManager(std::string root);
		~FileResourceManager();

		virtual IResourceRef GetResourceByName(std::string name);
		virtual IResourceRef GetResourceByUUID(Guid uuid);
	private:
		std::string m_root;

	public:
		class FileResource : public IResource {
			friend class FileResourceManager;
		public:
			FileResource();
			~FileResource();
			virtual void* GetData();
			virtual size_t GetSize();

		protected:
			FileResource(void* data, size_t size);

		private:
			size_t m_size;
			void* m_data;
		};
	};
}

// exceptions
DEFINE_EXCEPTION(AssetLoadException, 1, "Cannot load asset");
