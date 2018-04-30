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
	class Module : public Referencable {
	public:

	    explicit Module(Ref<Module> parent = nullptr);
		virtual ~Module();

		virtual void Initialize() = 0;
		
		Ref<View> GetView() const { return m_view; }
		Ref<Module> GetParentModule() const { return m_parent; }

		Ref<Module> GetChildModule(const size_t id) { return m_children[id]; }
		size_t GetChildModuleCount() const { return m_children.size(); }

		Ref<Module> GetRootModule() { return m_parent ? m_parent->GetRootModule() : this; }

		void SetParentModule(const Ref<Module> parent) { m_parent = parent; }
		void AddChildModule(const Ref<Module> child) { m_children.push_back(child);}

		void SetView(const Ref<View>view) { m_view = view; }

	protected:
		Ref<Module> m_parent;
		std::vector<Ref<Module>> m_children;
		Ref<View> m_view;

	};

	// ----------------------------------------------------------------

	class View : public Referencable {
		friend class Roles::ViewRefreshQueue;
		friend class Module;
	public:
		View();
		virtual ~View();

		void RequestRefreshView(bool force);

		virtual void SetModel(Grafkit::IResource * modelResource) {};

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
