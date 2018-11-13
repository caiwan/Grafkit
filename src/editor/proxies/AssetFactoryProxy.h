#pragma once 

#include <cassert>
#include <qobject>

#include "core/asset.h"
#include "utils/asset/AssetFactory.h"

namespace Idogep {

	/*
	Searches fora an sset in QT Resources when an asset file is not found by the default
	asset factroy as a fallback
	*/

	class AssetFactoryProxy : public Grafkit::IAssetFactory {
	public:
	    explicit AssetFactoryProxy(const std::shared_ptr<IAssetFactory> & parentFactory);
	    ~AssetFactoryProxy();

	    bool PollEvents(Grafkit::IResourceManager *resman) override { return m_parentFactory->PollEvents(resman); }

	    Grafkit::StreamRef Get(std::string name) override;

	    filelist_t GetAssetList() override { return m_parentFactory->GetAssetList(); }
	    filelist_t GetAssetList(Grafkit::AssetFileFilter * filter) override { return m_parentFactory->GetAssetList(filter); }

	    void SetBasePath(const std::string& path) override;

	private:
		std::shared_ptr<IAssetFactory> m_parentFactory;
	};

}
