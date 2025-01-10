#ifndef RENDERER_H
#define RENDERER_H
#include "base.h"
#include "Graphics/Renderer/DirectX/D3D11_Renderer.h"
struct RenderConfig
{
    float screenRatio[2] ={1000.0f,0.3f}; // screenNear, screenDepth
    bool  vsync = true;
};
struct RenderState
{
    float color[4];
    char *cardName;
    int memory;
};

void Renderer_SetConfig(RenderConfig &);
bool Renderer_Initialize();
void Renderer_Render();
void Renderer_Shutdown();
void Renderer_ResetViewport();
void Renderer_SetupColorBackGround(RenderState&);
void Renderer_GetVideoCardInfo(RenderState&);
void Renderer_SetBackBufferRenderTarget();
CONST_RELEASE RenderState &Renderer_GetState();

CONST_RELEASE RenderConfig &Renderer_GetConfig();
#endif