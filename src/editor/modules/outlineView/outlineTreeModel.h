#pragma once 

#include "common.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "models/OutlineItems.h"

namespace Grafkit {
	class Animation;
	typedef Ref<Animation> AnimationRef ;

	class Scene;
	typedef Resource<Scene> SceneRes;
	typedef Ref<SceneRes> SceneResRef;
}

namespace Idogep {

	class ActorItem;
	class EntityItem;
	class SceneGraphItem;

	class Properties;

	typedef Ref<Properties> PropertiesRef;

	// -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -

	class SceneGraphItem : public TreeItem, public HasItemPropertiesRole
	{
	public:
		SceneGraphItem(Grafkit::SceneGraphRef & scenegraph, TreeItem *parentItem = nullptr);

		Grafkit::SceneGraphRef GetSceneGraph() const;
	private:
		Grafkit::SceneGraphRef m_scenegraph;
	};

	class ActorItem : public TreeItem , public HasItemAnimationsRole, public HasItemPropertiesRole
	{
	public:
		ActorItem(Grafkit::ActorRef &actor, TreeItem*parentItem);
	private:
		Grafkit::ActorRef m_actor;

	};

	// -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -


	class EntityItem : public TreeItem 
	{
	public:
		EntityItem(Grafkit::Entity3DRef entity, TreeItem *parentItem);
	private:
		Grafkit::Entity3DRef m_entity;
	};

	class ModelItem : public EntityItem , public HasItemAnimationsRole, public HasItemPropertiesRole
	{
	public:
		ModelItem(Grafkit::ModelRef model, TreeItem *parentItem);
	private:
		Grafkit::Entity3DRef m_entity;
	};

	class CameraItem : public EntityItem, public HasItemAnimationsRole, public HasItemPropertiesRole
	{
	public:
		CameraItem(Grafkit::CameraRef camera, TreeItem *parentItem);

	};

	class LightItem: public EntityItem {
	public:
		LightItem(Grafkit::LightRef light, TreeItem *parentItem);
	};

	// -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -

	
	class SceneModel : public TreeModel
	{
	public:
	    explicit SceneModel(const Grafkit::SceneResRef & scene);
		~SceneModel();

	private:
	    QStringList Header() override;
	    void Build(TreeItem * parentItem) override;

		void BuildActor(TreeItem * parentItem, Grafkit::ActorRef actor, int maxDepth = 4096);
		void BuildEntity(TreeItem * parentItem, Grafkit::Entity3DRef entity);

		Grafkit::SceneResRef m_scene;

		std::map<Grafkit::Actor*, ActorItem*> m_actorMap;
		std::map<Grafkit::Entity3D *, EntityItem*> m_entityMap;
	};

}
