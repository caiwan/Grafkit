#pragma once 

#include <vector>

#include "common.h"
#include "utils/Command.h"

namespace Grafkit {
	class IResource;
}

namespace Idogep {
	class View;
	class Module;
	class Mediator;

	namespace Roles {
		class ViewRefreshQueue;
	}

	// ----------------------------------------------------------------
	class Module : public virtual Referencable {
	public:

		Module();
		virtual ~Module();

		virtual void Initialize(Ref<Module> parent = nullptr) = 0;

		// qnd hack
		virtual void MediateSiblingModule(Ref<Module> other) = 0;
		
		Ref<View> GetView() { return m_view; }
		Ref<Module> GetParentModule() { return m_parent; }

		// Get children IMHO is not needed

	protected:
		void SetParentModule(Ref<Module> parent) { m_parent = parent; }
		void AddChildModule(Ref<Module> child) { m_children.push_back(child);}
		void SetView(Ref<View>view) { m_view = view; }

	private:
		Ref<Module> m_parent;
		std::vector<Module*> m_children;
		Ref<View> m_view;

	};

	// ----------------------------------------------------------------

	class View : public virtual Referencable {
		friend class Roles::ViewRefreshQueue;
		friend class Module;
	public:
		View();
		virtual ~View();

		void RequestRefreshView(bool force);

		virtual void SetModel(Grafkit::IResource * modelResource) = 0;

	protected:
		virtual void RefreshView(bool force) = 0;

	private:
		Roles::ViewRefreshQueue * m_refreshQueueObject;

	};

}

// ----------------------------------------------------------------	

#include <qobject.h>

namespace Idogep {

	namespace Roles {

		// Misc stuff 
		// This suposed to put the refresh ant the end of the message dispatching
		// queue

		class ViewRefreshQueue : public QObject {
			Q_OBJECT
		public:
			ViewRefreshQueue(View * const & view) : m_view(view) { }

		public /*slots*/:
			void refreshViewSlot();
		private:
			View * const & m_view;
		};

	}
}
