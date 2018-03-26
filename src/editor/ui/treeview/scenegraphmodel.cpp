#include "scenegraphmodel.h"

#include "render/SceneGraph.h"

#include "render/camera.h"
#include "render/model.h"
#include "render/Light.h"

#include <qdebug.h>

using namespace Grafkit;
using namespace Idogep;


Idogep::SceneGraphItem::SceneGraphItem(Grafkit::SceneGraphRef & scenegraph, TreeItem * parentItem) : m_scenegraph(scenegraph)
{
	setIcon(QPixmap(":/icons/scene.png").scaled(16, 16));
	m_itemData << QString::fromStdString(scenegraph->GetName());
}


// ------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------

Idogep::SceneGraphModel::SceneGraphModel()
{
}

Idogep::SceneGraphModel::~SceneGraphModel()
{
}

void Idogep::SceneGraphModel::ScenegraphChanged(Grafkit::SceneGraphRef & scene)
{
	m_scene = scene;
}


QStringList Idogep::SceneGraphModel::Header()
{
	return QStringList() << "Node"; //<< "V" << "W" << "Lock";
}

void Idogep::SceneGraphModel::Build(TreeItem * parentItem)
{
	if (m_scene.Invalid())
	{
		qDebug() << "scene == nullptr!";
		return;
	}

	// clear children if there are a couple
	//if (parentItem->child)

	TreeItem * item = new SceneGraphItem(m_scene, parentItem);
	parentItem->addChild(item);

	// add actors 
	BuildActor(item, m_scene->GetRootNode());


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

	if (dynamic_cast<Camera*>(entity.Get()) != nullptr)
		item->setIcon(QPixmap(":/icons/camera.png").scaled(16, 16));

	else if (dynamic_cast<Model*>(entity.Get()) != nullptr)
		item->setIcon(QPixmap(":/icons/model.png").scaled(16, 16));

	else if (dynamic_cast<Light*>(entity.Get()) != nullptr)
		item->setIcon(QPixmap(":/icons/light.png").scaled(16, 16));

}

Idogep::ActorItem::ActorItem(Grafkit::ActorRef & actor, TreeItem * parentItem)
{
}

Idogep::EntityItem::EntityItem(Grafkit::Entity3DRef & entity, TreeItem * parentItem)
{
}
