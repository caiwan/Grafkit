#pragma once

#include "common.h"
#include "utils/Command.h"

namespace Grafkit
{
    class IResource;
    class IResourceManager;
}

namespace Idogep
{
    class View;
    class Controller;
    class Module;

    namespace Roles
    {
        class ViewRefreshEvent;
    }
    // ----------------------------------------------------------------

    class View : public Referencable
    {
        friend class Roles::ViewRefreshEvent;
        friend class Controller;
    public:
        static Ref<View> SafeGetView(Grafkit::IResourceManager * const&  resourceManager, const std::string &name);

        View();
        virtual ~View();

        void RequestRefreshView(bool force);

    protected:
        virtual void RefreshView(bool force) = 0;
       

    private:
        Roles::ViewRefreshEvent* m_refreshQueueObject;
    };

    // ----------------------------------------------------------------
    class Controller : public Referencable
    {
    public:
        static Ref<Controller> SafeGetController(Grafkit::IResourceManager * const&  resourceManager, const std::string &name);

        Controller();
        virtual ~Controller();

        virtual void Initialize(Grafkit::IResourceManager * const&  resourceManager) = 0;
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

        class ViewRefreshEvent : public QObject
        {
            Q_OBJECT
        public:
            explicit ViewRefreshEvent(View* const & view);

            void QueueRefresh(bool isForce);

        public slots:
            void refreshViewSlot();
        private:
            View * m_view;

            size_t m_isQueued;
            bool m_isSetForce;

        };
    }
}
