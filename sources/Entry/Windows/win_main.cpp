#include "platforms.h"

#if PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "Entry/Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    bool result = Application_Initialize();
    if (result)
    {
        Application_Run();
    }
    Application_Shutdown();
    return 0;
}

#endif