#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN

#include "glad/wgl.h"
#include "glad/gl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HDC   ghDC; 
HWND  ghWnd;
HGLRC ghRC; 

bool SetupPixelFormal(HDC hdc);
void DrawScene(HDC hdc);
GLvoid InitializeGL(GLsizei, GLsizei);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    ghWnd = CreateWindowEx(
        0,                                                       // Optional window styles.
        CLASS_NAME,                                              // Window class
        L"Windows OpenGL backends",                              // Window text
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-setpixelformat

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    if (ghWnd == NULL)
    {
        return 0;
    }

    ShowWindow(ghWnd, nCmdShow);
    UpdateWindow(ghWnd);
    // Run the message loop.

    /* animation loop */
    MSG msg{};
    while (1)
    {
        /*
         *  Process all pending messages
         */

        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                return TRUE;
            }
        }
        DrawScene(ghDC);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT    ps; 
    RECT rect; 
 
    switch (uMsg)
    {
    case WM_CREATE:
        ghDC = GetDC(hwnd);
        if(!SetupPixelFormal(ghDC))
        {
            PostQuitMessage(0);
        }
        ghRC = wglCreateContext(ghDC);
        wglMakeCurrent(ghDC,ghRC);
        gladLoaderLoadWGL(ghDC);
        GetClientRect(hwnd,&rect);
        //InitializeGL(rect.right, rect.bottom); 
        break;

        case WM_PAINT: 
        BeginPaint(hwnd, &ps); 
        EndPaint(hwnd, &ps); 
        break; 
 
    case WM_SIZE: 
        GetClientRect(hwnd, &rect); 
        break; 
 
    case WM_CLOSE: 
        if (ghRC) 
            wglDeleteContext(ghRC); 
        if (ghDC) 
            ReleaseDC(hwnd, ghDC); 
        ghRC = 0; 
        ghDC = 0; 
 
        DestroyWindow (hwnd); 
        break; 
 
    case WM_DESTROY: 
        if (ghRC) 
            wglDeleteContext(ghRC); 
        if (ghDC) 
            ReleaseDC(hwnd, ghDC); 
 
        PostQuitMessage (0); 
        break; 
     
    case WM_KEYDOWN: 
        switch (wParam) { 
        } 
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool SetupPixelFormal(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd =
    {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
            PFD_TYPE_RGBA,                                              // The kind of framebuffer. RGBA or palette.
            32,                                                         // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24, // Number of bits for the depthbuffer
            8,  // Number of bits for the stencilbuffer
            0,  // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
    };
    int pixelformat;

    pixelformat = ChoosePixelFormat(hdc, &pfd);

    if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == 0)
    {
        MessageBox(NULL, L"ChoosePixelFormat failed", L"Error", MB_OK);
        return FALSE;
    }

    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE)
    {
        MessageBox(NULL, L"SetPixelFormat failed", L"Error", MB_OK);
        return FALSE;
    }

    return TRUE;
}

void DrawScene(HDC hdc)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
    SwapBuffers(hdc);
}

GLvoid InitializeGL(GLsizei width, GLsizei height) 
{ 
    glClearDepth(1.0);
    //glEnable(GL_DEPTH_TEST); 
} 