#ifndef D3D11_RENDERER_IMPL_H
#define D3D11_RENDERER_IMPL_H
#include "platforms.h"
#if PLATFORM_WINDOWS
#include <d3d11.h>
#include <directxmath.h>
struct RenderState;
bool Renderer_D3D11_Initialize(RenderState &);
void Renderer_D3D11_Render(RenderState &);
void Renderer_D3D11_Shutdown(RenderState &);
void Renderer_D3D11_ResetViewport(RenderState &);
void Renderer_D3D11_SetBackBufferRenderTarget(RenderState &);
#endif
#endif