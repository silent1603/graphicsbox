#include "Render/Renderer.h"
#include <d3d11.h>
#include <directxmath.h>

struct RenderState
{
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain *swapChain = 0;
    ID3D11Device *device = 0;
    ID3D11DeviceContext *deviceContext = 0;
    ID3D11RenderTargetView *renderTargetView = 0;
    ID3D11Texture2D *depthStencilBuffer = 0;
    ID3D11DepthStencilState *depthStencilState = 0;
    ID3D11DepthStencilView *depthStencilView = 0;
    ID3D11RasterizerState *rasterState = 0;
    DirectX::XMMATRIX projectionMatrix;
    DirectX::XMMATRIX worldMatrix;
    DirectX::XMMATRIX orthoMatrix;
    D3D11_VIEWPORT viewport;
};

RenderState renderState;

struct RenderConfig
{
    bool vsync = true;
};

RenderConfig renderConfig;

void Renderer_SetConfig(RenderConfig &config) { renderConfig = config; }

bool Renderer_Initialize()
{
    HRESULT result;
    IDXGIFactory *factory;
    IDXGIAdapter *adapter;
}

void Renderer_Shutdown() {}

void Renderer_SetBackBufferRenderTarget() {}

void Renderer_ResetViewport() {}
CONST_RELEASE RenderState &Renderer_GetState() {}