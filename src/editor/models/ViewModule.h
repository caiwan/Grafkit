#pragma once 

#include <functional>
#include <qobject.h>

//#include "utils/Event.h"

#include "utils/Command.h"

namespace Idogep {
	class View;
	class ViewMediator;

	namespace Roles {
		class ViewRefreshQueue;
	}

	// ----------------------------------------------------------------

	class View {
		friend class Roles::ViewRefreshQueue;
	public:
		View();
		virtual ~View();

		void RequestRefreshView(bool force);

		virtual void SetModel(Ref<Referencable> model) = 0;

	protected:
		virtual void RefreshView(bool force) = 0;

	private:
		Roles::ViewRefreshQueue * m_refreshQueueObject;

	};

	// ----------------------------------------------------------------

	class ViewMediator {
	public:
		virtual void MediateViews(View *parent, View *child) = 0;
		virtual void MediateRoles(View *parent) = 0;
	};

	// ----------------------------------------------------------------
	// Misc stuff 
	// This suposed to put the refresh ant the end of the message dispatching
	// queue

	namespace Roles {

		// TODO: singleton

		class ViewRefreshQueue : public QObject {
			Q_OBJECT
		public:
			ViewRefreshQueue(View * const & view);

		public /*slots*/:
			void refreshViewSlot();
		private:
			View * m_view;
		};

	}
}