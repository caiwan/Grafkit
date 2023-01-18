#pragma once 

#include "utils/Command.h"
#include "utils/ViewModule.h"

namespace Idogep {

	class Document;
	class TreeItem;

	class TreeModel;
	
	class SceneGraphViewWidgetModel;

	class OutlineView : public View {
	public:
		OutlineView();

	    void SetModel(Grafkit::IResource * modelResource) override = 0;
		Event<TreeItem* const &> onItemSelected;
	};


    // TODO Item selected

	class OutlineModule : public Module , public EmitsCommandRole
	{

	public:
	    explicit OutlineModule(Ref<Module> parent);
		virtual ~OutlineModule();
		
	    void Initialize() override;

		void DocumentChangedEvent(Document* const & document);

		Event<TreeItem* const &> onItemOpened;
		Event<TreeItem* const &> onItemSelected;

	private:
		void ItemSelectedEvent(TreeItem* const & item);
	private:
		OutlineView * m_myView;
		
		Ref<SceneGraphViewWidgetModel> m_myModel;
		TreeModel* m_modelBuilder;
	};

}
