#include "fwzSetup.h"
#include "application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    fwzSettings setup;
    setup.hInstance = hInstance;

    setup.scrBPP = 32;
    setup.nVsync = 1;
    setup.nMultisample = 0;

    setup.nAlwaysOnTop = 1;
    setup.scrWidth = 800;
    setup.scrHeight = 600;
    setup.nWindowed = 1;
    if (!OpenSetupDialog(&setup))
        return -1;

    Application app(setup);
    return app.Execute();
}
