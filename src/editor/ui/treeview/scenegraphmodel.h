#pragma once 

#include <list>

#include "render/Scene.h"

#include "treeitem.h"
#include "treemodel.h"


namespace Idogep {

	class ActorItem;
	class EntityItem;
	class SceneGraphItem;

	// ... 

	class SceneGraphItem : public TreeItem
	{
	public:
		SceneGraphItem(Grafkit::SceneResRef &scene, TreeItem *parentItem = nullptr);
		
	};

	class SceneItem : public TreeItem
	{

	};

	class ActorItem : public TreeItem {

	};

	class EntityItem : public TreeItem {

	};

	// -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -

	class SceneGraphModel : public TreeModel
	{
	public:
		SceneGraphModel();
		~SceneGraphModel();

		void AddScene(Grafkit::SceneResRef &scene);

	private:
		virtual QStringList Header();
		virtual	void Build(TreeItem * parentItem);

		std::list<Grafkit::SceneResRef> m_scenes;
	};

}
