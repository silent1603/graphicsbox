#ifndef RENDERER_H
#define RENDERER_H
#include "base.h"
#include "Graphics/Renderer/DirectX/D3D11_Renderer.h"

enum RendererBackendType : int
{
    NULL_RENDERER = 0,
    DIRECT_X,
    VULKAN,
    OPENGL,
    WEBGL,
    WEBGPU,
    OPENGLES,
    METAL,
    BACKEND_RENDERER_SIZE
};

enum class RendererBackendFlagType : int
{
    NULL_RENDERER = 0x01, // 0000 0001
    DIRECT_X = 0x02,      // 0000 0010
    VULKAN = 0x04,        // 0000 0100
    OPENGL = 0x08,        // 0000 1000
    WEBGL = 0x10,         // 0001 0000
    WEBGPU = 0x20,        // 0010 0000
    OPENGLES = 0x40,      // 0100 0000
    METAL = 0x80,         // 1000 0000
    RendererBackendFlagType_SIZE = 8,
};

enum RendererFlagType : int
{
    VSYNC_ENABLE = 1,
};
struct RenderConfig
{
    float screenRatio[2] = {1000.0f, 0.3f}; // screenNear, screenDepth
    int   flag = 0 | RendererFlagType::VSYNC_ENABLE;
};

struct RenderState
{
    float backGroundColor[4];
    char  cardName[125];
    int   memory;
    int   rendererSupportedBackend[2]; // 0 : current render backend , 1 :
                                       // avaiable renderer backend
    D3D11RenderState d3d11RenderState;
    RenderConfig     rendererBackendConfig;
};

struct RenderFunction
{
    bool (*Renderer_Initialize)(RenderState &);
    void (*Renderer_Render)(RenderState &);
    void (*Renderer_Shutdown)(RenderState &);
    void (*Renderer_ResetViewport)(RenderState &);
    void (*Renderer_SetBackBufferRenderTarget)(RenderState &);
};

void Renderer_SetConfig(RenderConfig&);
void Renderer_SetupColorBackGround(float color[4]);
bool Renderer_Initialize();
void Renderer_Render();
void Renderer_Shutdown();

CONST_RELEASE RenderState  &Renderer_GetState();
CONST_RELEASE RenderConfig &Renderer_GetConfig();
#endif