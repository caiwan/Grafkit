#pragma once

#include <list>
#include <string>
#include <set>

#include "exceptions.h"
#include "reference.h"

#include "asset.h"

namespace Grafkit {

	// ----------
	/// An interface to create filters over resources
	class AssetFileFilter
	{
	public:
		AssetFileFilter(const char ** const & extensions, size_t count);
		virtual ~AssetFileFilter() {}

		/**
		Trims out filepath, filename, and extension (if any) from the given full filename
		*/
		static int trimpath(std::string fullpath, std::string &path, std::string &filename, std::string &extension);

		/// 
		int isFileInfilter(std::string path);

	private:
		typedef std::set<std::string> extFilter_t;
		extFilter_t m_filterExtensions;

	};

	/// An interface for resource obtaining mechnaism
	class IAssetFactory
	{
	public:
		IAssetFactory() {}
		// IAssetFactory(const char ** const & extensions, size_t count);

		virtual ~IAssetFactory() {}

	public:

		typedef std::list<std::string> filelist_t;

		virtual IAssetRef Get(std::string name) = 0;
		//virtual IAssetRef Get(Guid uuid) = 0;

		virtual filelist_t GetAssetList() { return std::list<std::string>(); };
		virtual filelist_t GetAssetList(AssetFileFilter * filter) { return std::list<std::string>(); }


	};

	// ==============================================================================
}

// exceptions
DEFINE_EXCEPTION(AssetLoadException, 1, "Cannot load asset");