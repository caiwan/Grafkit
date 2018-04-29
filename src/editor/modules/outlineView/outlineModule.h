#pragma once 

#include "ViewModule.h"

namespace Idogep {

	class TreeItem;

	class OutlineView : public View {
	public:
		OutlineView();

		virtual void SetModel(Grafkit::IResource * modelResource) = 0;

		Event<TreeItem*> onItemSelected;

	};

	class OutlineModule : public Module {

	public:
		OutlineModule(Ref<Module> parent);
		virtual void Initialize();

		// valahol itt kellene epitgetni a treeview-t, majd a vegen atadni a view iranyaba

		virtual void MediateSiblingModule(Ref<Module> other);

	private:
		OutlineView *m_myView;
	};

}