#pragma once 

#include <list>

#include "common.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

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

	// shall we have 
	// multiple animations maybe?
	class HasItemAnimationsRole {
	public :
		HasItemAnimationsRole();
		~HasItemAnimationsRole();

		void SetAnimation(Grafkit::AnimationRef animation);
		Grafkit::AnimationRef GetAnimation() { return m_animation; }

	protected:
		Grafkit::AnimationRef m_animation;
	};

	class HasItemPropertiesRole {
	public:
		HasItemPropertiesRole();
		~HasItemPropertiesRole();
	protected:
		Ref<Properties> m_properties;
	};

	// -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -

	class SceneGraphItem : public TreeItem, public HasItemPropertiesRole
	{
	public:
		SceneGraphItem(Grafkit::SceneGraphRef &scene, TreeItem *parentItem = nullptr);

		Grafkit::SceneGraphRef GetSceneGraph() { return m_scenegraph; }

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
		CameraItem(Grafkit::CameraRef entity, TreeItem *parentItem);

	};

	class LightItem: public EntityItem {
	public:
		LightItem(Grafkit::LightRef entity, TreeItem *parentItem);
	};

	// -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -

	
	class SceneModel : public TreeModel
	{
	public:
		SceneModel(Grafkit::SceneResRef &scene);
		~SceneModel();

	private:
		virtual QStringList Header();
		virtual	void Build(TreeItem * parentItem);

		void BuildActor(TreeItem * parentItem, Grafkit::ActorRef parentActor, int maxDepth = 4096);
		void BuildEntity(TreeItem * parentItem, Grafkit::Entity3DRef entity);

		Grafkit::SceneResRef m_scene;

	private:
		std::map<Grafkit::Actor*, ActorItem*> m_actorMap;
		std::map<Grafkit::Entity3D *, EntityItem*> m_entityMap;
	};

}
