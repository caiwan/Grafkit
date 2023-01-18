#pragma once 

#include "common.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "models/OutlineItems.h"
#include "scene/scene.h"

namespace GkDemo {
    class Demo;
    typedef Ref<Demo> DemoRef;
}

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

    class DemoItem : public TreeItem, public ItemHasAnimationsRole, public HasItemPropertiesRole
    {
    public:
        explicit DemoItem(const GkDemo::DemoRef& demo, TreeItem* parentItem = nullptr);

        GkDemo::DemoRef GetDemo() const;

        private:
            GkDemo::DemoRef m_demo;
    };

	class SceneGraphItem : public TreeItem, public HasItemPropertiesRole
	{
	public:
	    explicit SceneGraphItem(Grafkit::SceneGraphRef & scenegraph, TreeItem *parentItem = nullptr);

		Grafkit::SceneGraphRef GetSceneGraph() const;
	private:
		Grafkit::SceneGraphRef m_scenegraph;
	};

	class ActorItem : public TreeItem , public ItemHasAnimationsRole, public HasItemPropertiesRole
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
		explicit EntityItem(Grafkit::Entity3DRef entity, TreeItem *parentItem);
	private:
		Grafkit::Entity3DRef m_entity;
	};

	class ModelItem : public EntityItem , public ItemHasAnimationsRole, public HasItemPropertiesRole
	{
	public:
        explicit ModelItem(Grafkit::ModelRef model, TreeItem *parentItem);
	private:
		Grafkit::Entity3DRef m_entity;
	};

	class CameraItem : public EntityItem, public ItemHasAnimationsRole, public HasItemPropertiesRole
	{
	public:
        explicit CameraItem(Grafkit::CameraRef camera, TreeItem *parentItem);

	};

	class LightItem: public EntityItem {
	public:
        explicit LightItem(Grafkit::LightRef light, TreeItem *parentItem);
	};

	// ----------------------------------------------------------------------------------------------------

	
	class DemoModel : public TreeModel
	{
	public:
	    explicit DemoModel(const GkDemo::DemoRef & scene);
		~DemoModel();

	private:
	    QStringList Header() override;
	    void BuildAnimations(const Grafkit::HasAnimationsRole*const& container);
	    void BuildSceneGraph(TreeItem* parentItem, Grafkit::SceneGraphRef sceneGraph);
	    void Build(TreeItem * parentItem) override;

		void BuildActor(TreeItem * parentItem, Grafkit::ActorRef actor, int maxDepth = 4096);
		void BuildEntity(TreeItem * parentItem, Grafkit::Entity3DRef entity);

        GkDemo::DemoRef m_demo;

		std::map<Grafkit::Actor*, ActorItem*> m_actorMap;
		std::map<Grafkit::Entity3D *, EntityItem*> m_entityMap;
	};

}
