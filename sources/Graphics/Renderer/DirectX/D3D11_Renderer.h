#ifndef D3D11_RENDERER_H
#define D3D11_RENDERER_H
#include "platforms.h"
#if PLATFORM_WINDOWS
#include <d3d11.h>
#include <directxmath.h>
#endif
struct D3D11RenderState
{
#if PLATFORM_WINDOWS
    float                    backGroundColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    int                      videoCardMemory;
    char                     videoCardDescription[128];
    IDXGISwapChain          *swapChain = 0;
    ID3D11Device            *device = 0;
    ID3D11DeviceContext     *deviceContext = 0;
    ID3D11RenderTargetView  *renderTargetView = 0;
    ID3D11Texture2D         *depthStencilBuffer = 0;
    ID3D11DepthStencilState *depthStencilState = 0;
    ID3D11DepthStencilView  *depthStencilView = 0;
    ID3D11RasterizerState   *rasterState = 0;
    DirectX::XMMATRIX        projectionMatrix;
    DirectX::XMMATRIX        worldMatrix;
    DirectX::XMMATRIX        orthoMatrix;
    D3D11_VIEWPORT           viewport;
#endif
};
#endif