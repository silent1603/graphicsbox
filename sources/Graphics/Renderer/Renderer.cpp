#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Renderer/DirectX/D3D11_Renderer_Imp.h"
local_variable RenderState    renderState;
local_variable RenderConfig   renderConfig;
local_variable RenderFunction rendererBackendFunctions[RendererBackendType::BACKEND_RENDERER_SIZE];

void Renderer_SetupColorBackGround(float color[4])
{
    renderState.backGroundColor[0] = color[0];
    renderState.backGroundColor[1] = color[1];
    renderState.backGroundColor[2] = color[2];
    renderState.backGroundColor[3] = color[3];
}

bool Renderer_Initialize()
{
    renderState.rendererSupportedBackend[0] =
        (int)RendererBackendType::DIRECT_X;
    {
        rendererBackendFunctions[(int)RendererBackendType::DIRECT_X]
            .Renderer_Initialize = Renderer_D3D11_Initialize;
        rendererBackendFunctions[(int)RendererBackendType::DIRECT_X]
            .Renderer_Render = Renderer_D3D11_Render;
        rendererBackendFunctions[(int)RendererBackendType::DIRECT_X]
            .Renderer_ResetViewport = Renderer_D3D11_ResetViewport;
        rendererBackendFunctions[(int)RendererBackendType::DIRECT_X]
            .Renderer_SetBackBufferRenderTarget =
            Renderer_D3D11_SetBackBufferRenderTarget;
        rendererBackendFunctions[(int)RendererBackendType::DIRECT_X]
            .Renderer_Shutdown = Renderer_D3D11_Shutdown;
    }

    return rendererBackendFunctions[renderState.rendererSupportedBackend[0]]
        .Renderer_Initialize(renderState);
}

void Renderer_Render()
{
    rendererBackendFunctions[renderState.rendererSupportedBackend[0]]
        .Renderer_Render(renderState);
}

void Renderer_Shutdown()
{
    rendererBackendFunctions[renderState.rendererSupportedBackend[0]]
        .Renderer_Shutdown(renderState);
}

void Renderer_SetConfig(RenderConfig &config) { renderConfig = config; }

inline CONST_RELEASE RenderState  &Renderer_GetState() { return renderState; }
inline CONST_RELEASE RenderConfig &Renderer_GetConfig() { return renderConfig; }