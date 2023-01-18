#pragma once

#include <string>
#include <list>
#include <set>
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

	// ----------
	/// An interface to create filters over resources
	class ResourceFilter
	{
	public:
		ResourceFilter(const char ** const & extensions, size_t count);
		virtual ~ResourceFilter() {}

		int isFileInfilter(std::string path);

	private:
		typedef std::set<std::string> extFilter_t;
		extFilter_t m_filterExtensions;

	};

	// ----------
	
	typedef std::list<std::string> filelist_t;

	/// An interface for resource obtaining mechnaism
	class IResourceFactory 
	{
	public:
		IResourceFactory() {}
		// IResourceFactory(const char ** const & extensions, size_t count);
		
		virtual ~IResourceFactory() {}

	public:
		virtual IResourceRef GetResourceByName(std::string name) = 0;
		virtual IResourceRef GetResourceByUUID(Guid uuid) = 0;

		virtual filelist_t GetResourceList() { return std::list<std::string>(); };
		virtual filelist_t GetResourceList(ResourceFilter * filter) { return std::list<std::string>(); }


	};

	// ==============================================================================

	// loads resources from file
	class FileResourceManager : public IResourceFactory
	{
	public:
		class FileResource;

	public:
		//FileResourceManager(const char* root);
		FileResourceManager(std::string root);
		~FileResourceManager();

		virtual IResourceRef GetResourceByName(std::string name);
		virtual IResourceRef GetResourceByUUID(Guid uuid);

		virtual filelist_t GetResourceList();
		virtual filelist_t GetResourceList(ResourceFilter * filter);

	private:
		std::string m_root;
		filelist_t m_dirlist;

	public:
		class FileResource : public IResource {
			friend class FileResourceManager;
		public:
			FileResource(): m_size(0), m_data(nullptr){}
			~FileResource();
			virtual void* GetData() { return m_data; }
			virtual size_t GetSize() { return m_size; }

		protected:
			FileResource(void* data, size_t size): m_data(data), m_size(size) {}

		private:
			size_t m_size;
			void* m_data;
		};
	};
}

// exceptions
DEFINE_EXCEPTION(AssetLoadException, 1, "Cannot load asset");
