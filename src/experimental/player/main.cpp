#include "core/system.h"

using namespace Grafkit;

class Application : public System
{
public:
    int init() override;
    int mainloop() override;
    void release() override;
};

int Application::init() {
    return 0;
}

int Application::mainloop() {
    return 1;
}

void Application::release() {
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    Application app;
    return app.execute();
}
