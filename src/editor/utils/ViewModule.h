#pragma once

#include <vector>

#include "common.h"
#include "utils/Command.h"

namespace Grafkit
{
	class IResource;
}

namespace Idogep
{
	class View;
	class Controller;
	class Module;

	namespace Roles
	{
		class ViewRefreshQueue;
	}
	// ----------------------------------------------------------------

	class View : public Referencable
	{
		friend class Roles::ViewRefreshQueue;
		friend class Controller;
	public:
		explicit View() = default;
		explicit View(Ref<Controller> parentModule);
		virtual ~View();

		void RequestRefreshView(bool force);

		Ref<Controller> GetModule() const { return m_module; }
		void SetModule(const Ref<Controller>& module) { m_module = module; }

	protected:
		virtual void RefreshView(bool force) = 0;
		Ref<Controller> m_module;

	private:
		//Roles::ViewRefreshQueue* m_refreshQueueObject;
	};

	// ----------------------------------------------------------------
	class Controller : public Referencable
	{
	public:

		explicit Controller(Ref<Controller> parent = nullptr);
		virtual ~Controller();

		virtual void Initialize() = 0;

		Ref<View> GetView() const { return m_view; }
		Ref<Controller> GetParentModule() const { return m_parent; }

		Ref<Controller> GetChildModule(const size_t id) { return m_children[id]; }
		size_t GetChildModuleCount() const { return m_children.size(); }

		Ref<Controller> GetRootModule() { return m_parent ? m_parent->GetRootModule() : this; }

		void SetParentModule(const Ref<Controller> parent) { m_parent = parent; }
		void AddChildModule(const Ref<Controller> child) { m_children.push_back(child); }

		void SetView(const Ref<View> view) { m_view = view; m_view->SetModule(this); }

	protected:
		Ref<Controller> m_parent;
		std::vector<Ref<Controller>> m_children;
		Ref<View> m_view;
	};

    class Module
    {
        // ... 
    };

}

// ----------------------------------------------------------------	

// this thing must be solved in some other way 

#include <qobject.h>

namespace Idogep {
	namespace Roles
	{
		// Misc stuff 
		// This suposed to put the refresh ant the end of the message dispatching
		// queue

		class ViewRefreshQueue : public QObject
		{
			Q_OBJECT
		public:
		    explicit ViewRefreshQueue(View* const & view)
				: m_view(view)
			{
			}

			public slots :
				void refreshViewSlot() const;
		private:
			View * const & m_view;
		};
	}
}
