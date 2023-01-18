#pragma once 

#include "utils/Command.h"
#include "utils/ViewModule.h"

namespace Idogep {

	class Document;
	class TreeItem;

	class TreeModel;

	class SceneGraphViewWidgetModel;

	class OutlineModule;
	class OutlineView;

	class OutlineModule : public Module, public EmitsCommandRole
	{

	public:
		explicit OutlineModule(Ref<Module> parent);
		virtual ~OutlineModule();

		void Initialize() override;

		void DocumentChangedEvent(Document * const & document);

		Event<TreeItem * const> onItemSelected;
		
	private:
		OutlineView * m_myView;

		SceneGraphViewWidgetModel* m_myModel;
		TreeModel* m_modelBuilder;
	};

	class OutlineView : public View {
	public:
		OutlineView() = default;
		virtual void SetModel(SceneGraphViewWidgetModel * modelResource) = 0;
	};

}
