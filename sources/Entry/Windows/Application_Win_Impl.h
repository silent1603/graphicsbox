#ifndef APPLICATION_WIN_IMPL_H
#define APPLICATION_WIN_IMPL_H
#include "platforms.h"
#include "Entry/Application.h"
struct AppState
{
#if PLATFORM_WINDOWS
    HWND      hWindow = 0;
    HINSTANCE hInstance = 0;
#endif
    bool      keys[256] = {false};
    bool      running = false;
};
struct AppConfig
{
    float    screenProperties[2] = {1000.0f, 0.3f};
#if PLATFORM_WINDOWS
    LPCSTR   applicationName = "graphicbox";
#endif
    uint16_t screenSize[2] = {1024, 768};
    bool     fullscreen = false;
};

#endif