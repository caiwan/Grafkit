#pragma once

#include <string>

#include "Asset.h"
#include "AssetFactory.h"

namespace Grafkit {

	/** 
		File event watcher interface a live reloadingohz
	*/
	class IFileEventWatch {
	public:
		IFileEventWatch() {}
		virtual ~IFileEventWatch() {}

		virtual void Poll(IResourceManager *resman) = 0;
	};

	/**
		Filebol allit elo asseteket	
	*/
	class FileAssetFactory : public IAssetFactory
	{
	public:
		class FileAsset;

	public:
		FileAssetFactory(std::string root);
		~FileAssetFactory();

		virtual IAssetRef Get(std::string name);

		virtual filelist_t GetAssetList();
		virtual filelist_t GetAssetList(AssetFileFilter * filter);

		virtual void PollEvents(IResourceManager *resman);

	private:
		std::string m_root;
		filelist_t m_dirlist;

		IFileEventWatch *m_eventWatcher;

	public:
		class FileAsset : public IAsset {
			friend class FileAssetFactory;
		public:
			FileAsset() : m_size(0), m_data(nullptr) {}
			~FileAsset();
			virtual void* GetData() { return m_data; }
			virtual size_t GetSize() { return m_size; }

		protected:
			FileAsset(void* data, size_t size) : m_data(data), m_size(size) {}

		private:
			size_t m_size;
			void* m_data;
		};
	};
}
