#pragma once 

#include <qobject.h>

#include "core/asset.h"
#include "utils/asset/AssetFactory.h"

namespace Idogep {

	/*
	Searches fora an sset in QT Resources when an asset file is not found by the default
	asset factroy as a fallback
	*/

	class AssetFactoryProxy : public Grafkit::IAssetFactory {
	public:
	    explicit AssetFactoryProxy(IAssetFactory * const & parentFactory);

	    bool PollEvents(Grafkit::IResourceManager *resman) override { return m_parentFactory->PollEvents(resman); }

	    Grafkit::StreamRef Get(std::string name) override;

	    filelist_t GetAssetList() override { return m_parentFactory->GetAssetList(); }
	    filelist_t GetAssetList(Grafkit::AssetFileFilter * filter) override { return m_parentFactory->GetAssetList(filter); }

	    void SetBasePath(const std::string& path) override;

	private:
		IAssetFactory * const & m_parentFactory;
	};

	// --- 
	// Wrapper class for QT Resource 
	class QResourceAsset : public Grafkit::IStream {
	public:
		QResourceAsset(QByteArray &data) : IStream(), m_data(data) {}

	    void* GetData() const override { return (void*)m_data.data(); }
	    size_t GetSize() const override { return m_data.size(); }

	private:
		QByteArray m_data;
	};

}
