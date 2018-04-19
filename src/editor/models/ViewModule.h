#pragma once 

#include <qobject.h>

#include "event.h"
#include "command.h"

namespace Idogep {

class View;

namespace Role {
class CanRefreswViewRole{
};

class CanEncapulateViews {
};
	class ViewRefreshQueue;

}

	class View {
		friend class ViewRefreshQueue;
	public:
		View();
		virtual ~View();

		void RequestRefreshView(bool force);

	protected:
		virtual void RefreshView(bool force) = 0;

		#if 0
	private:
		ViewRefreshQueue * m_refreshQueueObject;
	#endif
	};

	namespace Role {
	// TODO: implement
	/// ... 
	#if 0
	class ViewRefreshQueue : public QObject {
		Q_OBJECT
	public:
		ViewRefreshQueue(View * const & view);
	public /*slots*/:
		void refreshViewSlot();
	private:
		View * m_view;
	};
#endif
	}
}