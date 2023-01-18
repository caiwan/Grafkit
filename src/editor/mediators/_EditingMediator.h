#pragma once 

#include "common.h"
#include "utils/Event.h"

#include "ViewModule.h"

namespace Idogep {

	class CommandStack;
	class Editor;

	namespace Roles {
		class ManageCommandStackRole {
		public:
			ManageCommandStackRole();

			Event<> onUndo;
			Event<> onRedo;

			void CommandStackChangedEvent(CommandStack * const & stack);

		protected:
			virtual void ToggleUndo(bool enabled) = 0;
			virtual void ToggleRedo(bool enabled) = 0;

			void ConnectCommandStackEvents(CommandStack * const & stack);
		};

		class ManageFileOperationsRole {
		public :
			Event<> onOpen;
			Event<> onSave;
			Event<> onNew;

			// dirty check maybe?
		};
	}

	class EditorView : public View,
		public Roles::ManageCommandStackRole,
		public Roles::ManageFileOperationsRole
	{
	public:
		EditorView();
		// on exit? 

		virtual void Initialize() = 0;

		virtual void SetModel(Grafkit::IResource * modelResource) = 0;
	};
}