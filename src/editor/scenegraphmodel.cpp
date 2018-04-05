#include "scenegraphmodel.h"

#include "render/SceneGraph.h"

#include "render/camera.h"
#include "render/model.h"
#include "render/Light.h"

#include "animation/actoranimation.h"

#include <qdebug.h>

using namespace Grafkit;
using namespace Idogep;


// ------------------------------------------------------------------------------------------

#include "animation/animation.h"

Idogep::HasItemAnimationsRole::HasItemAnimationsRole()
{
}

Idogep::HasItemAnimationsRole::~HasItemAnimationsRole()
{
}

void Idogep::HasItemAnimationsRole::SetAnimation(Grafkit::AnimationRef animation)
{
	m_animation = animation;
}

// ------------------------------------------------------------------------------------------

#include "property.h"

Idogep::HasItemPropertiesRole::HasItemPropertiesRole()
{
}

Idogep::HasItemPropertiesRole::~HasItemPropertiesRole()
{
}

// ------------------------------------------------------------------------------------------

Idogep::SceneGraphItem::SceneGraphItem(Grafkit::SceneGraphRef & scenegraph, TreeItem * parentItem) : m_scenegraph(scenegraph)
{
	setIcon(QPixmap(":/icons/scene.png").scaled(16, 16));
	m_itemData << QString::fromStdString(scenegraph->GetName());
}


// ------------------------------------------------------------------------------------------

Idogep::ActorItem::ActorItem(Grafkit::ActorRef & actor, TreeItem * parentItem)
{
	setIcon(QPixmap(":/icons/node.png").scaled(16, 16));
	m_itemData << QString::fromStdString(actor->GetName());
}

// ------------------------------------------------------------------------------------------

Idogep::EntityItem::EntityItem(Grafkit::Entity3DRef entity, TreeItem * parentItem) : m_entity(entity)
{
	setIcon(QPixmap(":/icons/model.png").scaled(16, 16));
	m_itemData << QString::fromStdString(entity->GetName());

}

Idogep::ModelItem::ModelItem(Grafkit::ModelRef model, TreeItem * parentItem) :
	EntityItem(dynamic_cast<Model*>(model.Get()), parentItem)
{
	setIcon(QPixmap(":/icons/model.png").scaled(16, 16));
	m_itemData << QString::fromStdString(model->GetName());
}

Idogep::CameraItem::CameraItem(Grafkit::CameraRef camera, TreeItem * parentItem) :
	EntityItem(dynamic_cast<Entity3D*>(camera.Get()), parentItem)
{
	setIcon(QPixmap(":/icons/camera.png").scaled(16, 16));
	m_itemData << QString::fromStdString(camera->GetName());

}

Idogep::LightItem::LightItem(Grafkit::LightRef light, TreeItem * parentItem) :
	EntityItem(dynamic_cast<Entity3D*>(light.Get()), parentItem)
{
	setIcon(QPixmap(":/icons/light.png").scaled(16, 16));
	m_itemData << QString::fromStdString(light->GetName());
}


// ------------------------------------------------------------------------------------------

#include "animation/scene.h" 

Idogep::SceneModel::SceneModel(Grafkit::SceneResRef &scene)
{
	m_scene = scene;
}

Idogep::SceneModel::~SceneModel()
{
}

QStringList Idogep::SceneModel::Header()
{
	return QStringList() << "Node"; //<< "V" << "W" << "Lock";
}

void Idogep::SceneModel::Build(TreeItem * parentItem)
{
	if (m_scene.Invalid() || m_scene->Invalid())
	{
		qDebug() << "scene == nullptr!";
		return;
	}

	// clear children if there are a couple
	//if (parentItem->child)

	SceneGraphRef sceneGraph = (*m_scene)->GetSceneGraph();

	TreeItem * item = new SceneGraphItem(sceneGraph, parentItem);
	parentItem->addChild(item);

	// add actors 
	BuildActor(item, sceneGraph->GetRootNode());

	// Add animation items if any

	for (size_t i = 0; i< (*m_scene)->GetAnimationCount(); i++ )
	{
		Animation * animation = (*m_scene)->GetAnimation(i);
		
		// --- 
		ActorAnimation * actorAnimation = dynamic_cast<ActorAnimation*>(animation);
		if (actorAnimation) {
			Actor * actor = actorAnimation->GetActor();
			auto it = m_actorMap.find(actor);
			if (it != m_actorMap.end()) {
				it->second->SetAnimation(animation);
			}
		}

		// masikra ugyanez:
		//EntityAnimation * entityAnimation = dynamic_cast<EntityAnimation*>(animation);
		// if (...) ... ;
	}

}

void Idogep::SceneModel::BuildActor(TreeItem * parentItem, Grafkit::ActorRef actor, int maxDepth)
{
	ActorItem * item = new ActorItem(actor, parentItem);
	parentItem->addChild(item);

	for (int i = 0; i < actor->GetChildrenCount(); i++) {
		BuildActor(item, actor->GetChild(i), maxDepth - 1);
	}

	for (int i = 0; i < actor->GetEntityCount(); i++) {
		BuildEntity(item, actor->GetEntity(i));
	}

	m_actorMap.insert_or_assign(actor, item);
}

void Idogep::SceneModel::BuildEntity(TreeItem * parentItem, Grafkit::Entity3DRef entity)
{
	QList<QVariant> rowData;

	EntityItem * item = nullptr;

	const CameraRef camera = dynamic_cast<Camera*>(entity.Get());
	const ModelRef model = dynamic_cast<Model*>(entity.Get());
	const LightRef light = dynamic_cast<Light*>(entity.Get());

	if (camera) {
		item = new CameraItem(camera, parentItem);
	}
	else if (light) {
		/*BuildCamera(item, camera);*/
		item = new CameraItem(camera, parentItem);
	}
	else if (model) {
		item = new ModelItem(model, parentItem);
	}

	if (item) {
		parentItem->addChild(item);
		m_entityMap.insert_or_assign(entity, item);
	}
}
