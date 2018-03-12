#pragma once 

#include <qobject.h>

#include "utils/Asset.h"
#include "utils/AssetFactory.h"

namespace Idogep {

	/*
	Searches fora an sset in QT Resources when an asset file is not found by the default
	asset factroy as a fallback
	*/

	class AssetFactoryProxy : public Grafkit::IAssetFactory {
	public:
		AssetFactoryProxy(Grafkit::IAssetFactory * const & parentFactory);

		virtual bool PollEvents(Grafkit::IResourceManager *resman) { return m_parentFactory->PollEvents(resman); }

		virtual Grafkit::IAssetRef Get(std::string name);

		virtual filelist_t GetAssetList() { return m_parentFactory->GetAssetList(); }
		virtual filelist_t GetAssetList(Grafkit::AssetFileFilter * filter) { return m_parentFactory->GetAssetList(filter); }

	private:
		IAssetFactory * const & m_parentFactory;
	};

	// --- 
	// Wrapper class for QT Resource 
	class QResourceAsset : public Grafkit::IAsset {
	public:
		QResourceAsset(QByteArray &data) : IAsset(), m_data(data) {}

		virtual void* GetData() { return m_data.data(); }
		virtual size_t GetSize() { return m_data.size(); }

	private:
		QByteArray m_data;
	};

}
