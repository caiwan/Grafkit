#pragma once

#include <string>

#include "Asset.h"
#include "AssetFactory.h"
#include "core/thread.h"

namespace Grafkit {

	/** 
		File event watcher interface a live reloadingohz
	*/
	class IFileEventWatch {
	public:
		IFileEventWatch() {}
		virtual ~IFileEventWatch() {}

		std::string PopFile() {
			MutexLocker lock(&m_queueMutex);
			std::string fn;
			if (!m_fileReloadList.empty()) {
				fn = m_fileReloadList.front();
				m_fileReloadList.pop_front();
			}
			return fn;
		}

		bool HasItems() {
			return !m_fileReloadList.empty();
		}

		virtual void Poll() = 0;

	protected:
		std::list<std::string> m_fileReloadList;
		Mutex m_queueMutex;
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
