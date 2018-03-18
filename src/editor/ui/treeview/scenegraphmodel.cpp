#include "scenegraphmodel.h"

Idogep::SceneGraphItem::SceneGraphItem(Grafkit::SceneResRef & scene, TreeItem * parentItem)
{
}


// ------------------------------------------------------------------------------------------

Idogep::SceneGraphModel::SceneGraphModel()
{
}

Idogep::SceneGraphModel::~SceneGraphModel()
{
}

void Idogep::SceneGraphModel::AddScene(Grafkit::SceneResRef & scene)
{
	m_scenes.push_back(scene);
}

QStringList Idogep::SceneGraphModel::Header()
{
	return QStringList() << "Node"; //<< "V" << "W" << "Lock";
}

void Idogep::SceneGraphModel::Build(TreeItem * parentItem)
{
	for (auto it = m_scenes.begin(); it != m_scenes.end(); it++) {
		//SceneGraphItem *item = new SceneGraphItem();
		QList<QVariant> rowData;
		rowData << QString::fromStdString(it->Get()->GetName());

		TreeItem * item = new TreeItem(rowData);
		parentItem->addChild(item);
	}
}


