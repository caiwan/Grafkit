#pragma once 

#include "utils/Command.h"
#include "utils/ViewModule.h"

namespace Idogep {

	class Document;
	class TreeItem;

	class SceneGraphViewWidgetModel;

	class OutlineView : public View {
	public:
		OutlineView();

		virtual void SetModel(Grafkit::IResource * modelResource) = 0;
		Event<TreeItem*> onItemSelected;
	};


	class OutlineModule : public Module , public EmitsCommandRole
	{

	public:
	    explicit OutlineModule(Ref<Module> parent);
	    void Initialize() override;

		void DocumentChangedEvent(Document* const & document);

	private:
		OutlineView * m_myView;
		SceneGraphViewWidgetModel * m_myModel;
	};

}