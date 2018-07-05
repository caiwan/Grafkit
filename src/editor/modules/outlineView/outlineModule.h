#pragma once 

#include "utils/Command.h"
#include "utils/ViewModule.h"

namespace GkDemo
{
    class Demo;
    class Context;
}

namespace Idogep {

	class Document;
	class TreeItem;

	class TreeModel;

	class SceneGraphViewWidgetModel;

	class OutlineModule;
	class OutlineView;

	class OutlineModule : public Controller, public EmitsCommandRole
	{

	public:
		explicit OutlineModule(Ref<Controller> parent);
		virtual ~OutlineModule();

		void Initialize() override;

		void DocumentChangedEvent(GkDemo::Demo * const & document);

		Event<TreeItem * const> onItemSelected;
		
	private:
		OutlineView * m_myView;

		SceneGraphViewWidgetModel* m_myModel;
		TreeModel* m_modelBuilder;
	};

	class OutlineView : public View {
	public:
		OutlineView();
		virtual void SetModel(SceneGraphViewWidgetModel * modelResource) = 0;
	};

}
