#include "platforms.h"
#if PLATFORM_WINDOWS
#include "DirectXMath.h"
#include "Entry/Application.h"
#include "d3d11.h"
#include <stdio.h>
struct AppState
{
    bool keys[256] = {false};
    HWND hwnd = 0;
    HINSTANCE hInstance = 0;
    bool running = false;
};

AppState appState;

struct AppConfig
{
    uint16_t screenSize[2] = {1024, 768};
    LPCSTR applicationName = "graphicbox";
    bool fullscreen = false;
    float screenProperties[2] = {1000.0f, 0.3f};
};

AppConfig appConfig;
LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam,
                                LPARAM lparam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
        default:
        {
            return MessageHandler(hwnd, umessage, wparam, lparam);
        }
    }
}

LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam,
                                LPARAM lparam)
{
    switch (umessage)
    {
        case WM_KEYDOWN:
        {
            // If a key is pressed send it to the input object so it can
            // record that state.
            appState.keys[(unsigned int)wparam] = true;
            return 0;
        }

        // Check if a key has been released on the keyboard.
        case WM_KEYUP:
        {
            // If a key is released then send it to the input object so it
            // can unset the state for that key.
            appState.keys[(unsigned int)wparam] = false;
            return 0;
        }

        // Any other messages send to the default message handler as our
        // application won't make use of them.
        default:
        {
            return DefWindowProc(hwnd, umessage, wparam, lparam);
        }
    }
}

void Application_SetConfig(AppConfig &config) { appConfig = config; }

bool Application_Initialize()
{
    bool result = true;
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;
    appState.hInstance = GetModuleHandle(NULL);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = appState.hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = appConfig.applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Error registering class", "Error",
                   MB_OK | MB_ICONERROR);
#if DEBUG_BUILD
        char msgbuf[256];
        sprintf(msgbuf,
                "Error in Function = WinMain() at line = %d, with error code = "
                "%X \n",
                __LINE__ - 3, (unsigned int)GetLastError());
        OutputDebugString(msgbuf);
#endif
        result = false;
    }

    // Setup the screen settings depending on whether it is running in full
    // screen or in windowed mode.
    if (appConfig.fullscreen)
    {
        // If full screen set the screen to maximum size of the users desktop
        // and 32bit.
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = GetSystemMetrics(SM_CXSCREEN);
        dmScreenSettings.dmPelsHeight = GetSystemMetrics(SM_CYSCREEN);
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields =
            DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        // Set the position of the window to the top left corner.
        posX = posY = 0;
    }
    else
    {

        // Place the window in the middle of the screen.
        posX = (GetSystemMetrics(SM_CXSCREEN) - appConfig.screenSize[0]) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - appConfig.screenSize[1]) / 2;
    }
    RECT size = {0, 0, appConfig.screenSize[0], appConfig.screenSize[1]};
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
    // Create the window with the screen settings and get the handle to it.
    appState.hwnd = CreateWindowEx(
        WS_EX_APPWINDOW, appConfig.applicationName, appConfig.applicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, posX, posY,
        size.right - size.left, size.bottom - size.top, NULL, NULL,
        appState.hInstance, NULL);
    SetLayeredWindowAttributes(appState.hwnd, NULL, 255, LWA_ALPHA);
    // Bring the window up on the screen and set it as main focus.
    ShowWindow(appState.hwnd, SW_SHOW);
    UpdateWindow(appState.hwnd);
    SetForegroundWindow(appState.hwnd);
    SetFocus(appState.hwnd);

    return result;
}

void Application_Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    appState.running = true;
    while (appState.running)
    {
        if (PeekMessage(&msg, appState.hwnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (appState.keys[VK_ESCAPE])
        {
            if (MessageBox(0, "Are you sure you want to exit?", "Really?",
                           MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
                appState.running = false;
            }
        }
    }

    if (msg.message == WM_QUIT)
    {
        appState.running = false;
    }
}
}

void Application_Shutdown()
{
    if (appConfig.fullscreen)
    {
        ChangeDisplaySettings(NULL, 0);
    }
    DestroyWindow(appState.hwnd);
    appState.hwnd = 0;
    UnregisterClass(appConfig.applicationName, appState.hInstance);
    appState.hInstance = NULL;
}

CONST_RELEASE AppState &Application_GetAppState() { return appState; }
#endif