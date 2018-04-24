#pragma once 

#include "common.h"
#include "utils/Event.h"
#include "models/ViewModule.h"

class QAction;

namespace Idogep{

	class TreeItem;

	namespace Roles {
	
		class MediateOutlineViewRole : public ViewMediator {
		public:
			MediateOutlineViewRole();
			void MediateViews(View *parent, View *child);
			void MediateRoles(View *parent);

		public:
			void ItemSelectedEvent(TreeItem *item);
			Event<Grafkit::AnimationRef> onAnimationSelected;

		};

		// -------------------------------------------------------

		class CommandStack;

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

		protected:
			QAction * m_undoAct;
			QAction * m_redoAct;
		};
	
	}
}