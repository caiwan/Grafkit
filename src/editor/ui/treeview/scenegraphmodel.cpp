#include "scenegraphmodel.h"

#include "render/camera.h"
#include "render/model.h"
#include "render/Light.h"

#include <qdebug.h>

using namespace Grafkit;

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
	for (auto scene = m_scenes.begin(); scene != m_scenes.end(); scene++) {
		if (scene->Invalid() || scene->Get()->Invalid())
		{
			qDebug() << "scene == nullptr!";
			continue;
		}
		//SceneGraphItem *item = new SceneGraphItem();
		QList<QVariant> rowData;
		rowData << QString::fromStdString(scene->Get()->GetName());

		TreeItem * item = new TreeItem(rowData);
		item->setIcon(QPixmap(":/icons/scene.png").scaled(16, 16));
		parentItem->addChild(item);

		// add actors 
		BuildActor(item, (**scene)->GetRootNode());

	}
}

void Idogep::SceneGraphModel::BuildActor(TreeItem * parentItem, Grafkit::ActorRef parentActor, int maxDepth)
{
	QList<QVariant> rowData;
	rowData << QString::fromStdString(parentActor->GetName());

	TreeItem * item = new TreeItem(rowData);
	item->setIcon(QPixmap(":/icons/node.png").scaled(16, 16));
	parentItem->addChild(item);

	for (int i = 0; i < parentActor->GetChildrenCount(); i++) {
		BuildActor(item, parentActor->GetChild(i), maxDepth - 1);
	}

	for (int i = 0; i < parentActor->GetEntityCount(); i++) {
		BuildEntity(item, parentActor->GetEntity(i));
	}
}

void Idogep::SceneGraphModel::BuildEntity(TreeItem * parentItem, Grafkit::Entity3DRef entity)
{
	QList<QVariant> rowData;
	rowData << QString::fromStdString(entity->GetName());

	TreeItem * item = new TreeItem(rowData);
	parentItem->addChild(item);

	if (dynamic_cast<Camera*>(entity.Get()) != nullptr )
		item->setIcon(QPixmap(":/icons/camera.png").scaled(16, 16));

	else if (dynamic_cast<Model*>(entity.Get()) != nullptr)
		item->setIcon(QPixmap(":/icons/model.png").scaled(16, 16));

	else if (dynamic_cast<Light*>(entity.Get()) != nullptr)
		item->setIcon(QPixmap(":/icons/light.png").scaled(16, 16));

}

