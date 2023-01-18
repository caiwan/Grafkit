#pragma once 

#include "utils/Event.h"
#include "utils/ViewModule.h"

namespace Idogep {

	class CommandStack;
	class Editor;

	namespace Roles {
		class ManageCommandStackRole {
		public:
			ManageCommandStackRole() = default;
			virtual ~ManageCommandStackRole() = default;

			Event<> onUndo;
			Event<> onRedo;

			void CommandStackChangedEvent(CommandStack * const & stack);

		protected:
			virtual void ToggleUndo(bool enabled) = 0;
			virtual void ToggleRedo(bool enabled) = 0;

			void ConnectCommandStackEvents(CommandStack * const & stack);
		};

		class ManageFileOperationsRole {
		public:
			ManageFileOperationsRole() = default;
		    virtual ~ManageFileOperationsRole() = default;

			Event<> onLoad;
			Event<> onSave;
			Event<> onNew;

			// dirty check maybe?

            virtual bool OpenFolder(std::string & outFilename) = 0;
		};
	}

	class EditorView : public View,
		public Roles::ManageCommandStackRole,
		public Roles::ManageFileOperationsRole
	{
	public:
	    explicit EditorView();
		virtual ~EditorView() = default;

	    // ... 

	};
}
