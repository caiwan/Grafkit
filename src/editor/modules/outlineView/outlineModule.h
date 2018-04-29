#pragma once 

#include "ViewModule.h"

namespace Idogep {

	class TreeItem;

	class OutlineView : public View {
	public:
		OutlineView();

		void SetModel(Ref<Referencable> model);

		Event<TreeItem*> onItemSelected;
		//Event<TreeItem*> onItemHighlighted;
	};

	class OutlineModule : public Module {

	public:
		OutlineModule();
		virtual void Initialize() = 0;

		virtual void MediateSiblingModule(Ref<Module> other);

	private:
		OutlineView m_myView;
	};

}