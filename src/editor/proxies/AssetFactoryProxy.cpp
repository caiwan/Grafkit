#include "AssetFactoryProxy.h"

#include <qfile.h>
#include <qdebug.h>

using namespace Grafkit;
using namespace Idogep;
using namespace FWdebugExceptions;

AssetFactoryProxy::AssetFactoryProxy(IAssetFactory * const & parentFactory) : IAssetFactory(), m_parentFactory(parentFactory)
{
}

StreamRef AssetFactoryProxy::Get(std::string name)
{
	StreamRef asset = nullptr;

	try {
		asset = m_parentFactory->Get(name);
	}
    catch (AssetLoadException &ex) {
		QString filename = QString(":/resources/") + QString::fromStdString(name);

		QFile file(filename);
		if (file.open(QIODevice::ReadOnly)) {
			QByteArray data = file.readAll();
			asset = new QResourceAsset(data);
		}
		else {
            throw ex;	// eh, ezt lehet nem igy kell majd itt kezelni
		}
	}

	return asset;
}

void AssetFactoryProxy::SetBasePath(const std::string& path) {
    m_parentFactory->SetBasePath(path);
}
