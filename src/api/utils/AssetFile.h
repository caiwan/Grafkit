#pragma once

#include <string>

#include "Asset.h"
#include "AssetFactory.h"

namespace Grafkit {
	// loads resources from file
	class FileAssetManager : public IAssetFactory
	{
	public:
		class FileAsset;

	public:
		//FileAssetManager(const char* root);
		FileAssetManager(std::string root);
		~FileAssetManager();

		virtual IAssetRef Get(std::string name);
		//virtual IAssetRef GetResourceByUUID(Guid uuid);

		virtual filelist_t GetAssetList();
		virtual filelist_t GetAssetList(AssetFileFilter * filter);

	private:
		std::string m_root;
		filelist_t m_dirlist;

	public:
		class FileAsset : public IAsset {
			friend class FileAssetManager;
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