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
	/// @todo ezt kulon fileba majd, ha lehet, vagy nem 
	/// An interface to create filters over resources
	class IResourceFilter
	{
	public:
		IResourceFilter() {}
		virtual ~IResourceFilter() {}

		virtual int isFileInfilter(std::string path) = 0;

	};

	class ResourceFilterExtension : IResourceFilter
	{
	public:
		ResourceFilterExtension(const char ** const & extensions, size_t count);
		virtual ~ResourceFilterExtension() {}

		int isFileInfilter(std::string path);

	private:
		typedef std::set<std::string> extFilter_t;
		extFilter_t m_filterExtensions;

	};

	// ----------

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

		typedef std::list<std::string> fileleist_t;
		virtual fileleist_t GetResourceList() { return std::list<std::string>(); };


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

		virtual std::list<std::string> GetResourceList();
		virtual std::list<std::string> GetResourceList(IResourceFilter * filter);

	private:
		std::string m_root;
		fileleist_t m_dirlist;

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
