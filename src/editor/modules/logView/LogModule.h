#pragma once

#include "utils/ViewModule.h"

namespace Idogep
{
    class LogView;

    class LogView : public View {
    public:
        explicit LogView(const Ref<Controller>& controller);

        virtual void UpdateLog(std::string log) = 0;
    };

    class LoggerProxy;

    class LogModule : public Controller
    {
    public:
        explicit LogModule(LoggerProxy* const & loggerProxy);
        ~LogModule();

        void Initialize(Grafkit::IResourceManager* const& resourceManager) override;

    private:
        Ref<LogView> m_myView;
        LoggerProxy* m_loggerProxy;
    };
}
