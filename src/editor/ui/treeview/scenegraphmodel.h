#pragma once 

#include <list>

//#include "animation/Scene.h"
//#include "render/anim"

#include "common.h"

#include "treeitem.h"
#include "treemodel.h"

namespace Idogep {

	class ActorItem;
	class EntityItem;
	class SceneGraphItem;

	// ... 

	class Properties;

	class HasItemPropertiesRole {
	private:
		Properties * m_poror;
	};

	// -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -

	class SceneGraphItem : public TreeItem
	{
	public:
		SceneGraphItem(Grafkit::SceneGraphRef &scene, TreeItem *parentItem = nullptr);

		Grafkit::SceneGraphRef GetSceneGraph() { return m_scenegraph; }

	private:
		Grafkit::SceneGraphRef m_scenegraph;
	};

	class ActorItem : public TreeItem {
	public:
		ActorItem(Grafkit::ActorRef &actor, TreeItem*parentItem);
	private:
		Grafkit::ActorRef m_actor;

	};

	// -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -


	class EntityItem : public TreeItem {
	public:
		EntityItem(Grafkit::Entity3DRef entity, TreeItem *parentItem);
	private:
		Grafkit::Entity3DRef m_entity;
	};

	class ModelItem : public EntityItem {
	public:
		ModelItem(Grafkit::ModelRef model, TreeItem *parentItem);
	private:
		Grafkit::Entity3DRef m_entity;
	};

	class CameraItem : public EntityItem {
	public:
		CameraItem(Grafkit::CameraRef entity, TreeItem *parentItem);

	};

	class LightItem: public EntityItem {
	public:
		LightItem(Grafkit::LightRef entity, TreeItem *parentItem);
	};

	// -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -

	class SceneGraphModel : public TreeModel
	{
	public:
		SceneGraphModel();
		~SceneGraphModel();

		void ScenegraphChanged(Grafkit::SceneGraphRef &scene);
		//void SceneChanged(Grafkit::SceneResRef &scene);

	private:
		virtual QStringList Header();
		virtual	void Build(TreeItem * parentItem);

		void BuildActor(TreeItem * parentItem, Grafkit::ActorRef parentActor, int maxDepth = 4096);
		void BuildEntity(TreeItem * parentItem, Grafkit::Entity3DRef entity);

		Grafkit::SceneGraphRef m_scene;
	};

}
