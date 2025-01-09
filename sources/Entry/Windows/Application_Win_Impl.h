#ifndef APPLICATION_WIN_IMPL_H
#define APPLICATION_WIN_IMPL_H
#include "Entry/Application.h"
#include "platforms.h"
struct AppState
{
    HWND      hWindow = 0;
    HINSTANCE hInstance = 0;
    bool      keys[256] = {false};
    bool      running = false;
};
struct AppConfig
{
    float    screenProperties[2] = {1000.0f, 0.3f};
    LPCSTR   applicationName = "graphicbox";
    uint16_t screenSize[2] = {1024, 768};
    bool     fullscreen = false;
};

#endif