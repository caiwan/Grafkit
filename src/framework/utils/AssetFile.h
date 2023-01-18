#pragma once

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

		bool HasItems() const {
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

	    explicit FileAssetFactory(std::string root);
		~FileAssetFactory();

	    IAssetRef Get(std::string name) override;

	    filelist_t GetAssetList() override;
	    filelist_t GetAssetList(AssetFileFilter * filter) override;

	    bool PollEvents(IResourceManager *resman) override;

	    void SetBasePath(const std::string& path) override;

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
		    void* GetData() const  override { return m_data; }
		    size_t GetSize() const override { return m_size; }

		protected:
			FileAsset(void* data, size_t size) : m_data(data), m_size(size) {}

		private:
			size_t m_size;
			void* m_data;
		};
	};
}
