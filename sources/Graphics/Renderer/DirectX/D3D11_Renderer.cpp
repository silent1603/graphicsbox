#include "Graphics/Renderer/DirectX/D3D11_Renderer.h"
#include "Entry/Windows/Application_Win_Impl.h"

local_variable RenderState  renderState;
local_variable RenderConfig renderConfig;

void Renderer_SetConfig(RenderConfig &config) { renderConfig = config; }

bool Renderer_Initialize()
{
    const AppState  &appState = Application_GetAppState();
    const AppConfig &appConfig = Application_GetAppConfig();

    HRESULT result;

    // Create a DirectX graphics interface factory.
    IDXGIFactory *factory;
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory);
    if (FAILED(result))
    {
        return false;
    }

    // Use the factory to create an adapter for the primary graphics interface
    // (video card).
    IDXGIAdapter *adapter;
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result))
    {
        return false;
    }

    // Enumerate the primary adapter output (monitor).
    IDXGIOutput *adapterOutput;
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result))
    {
        return false;
    }

    unsigned int numModes;
    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display
    // format for the adapter output (monitor).
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                               DXGI_ENUM_MODES_INTERLACED,
                                               &numModes, NULL);
    if (FAILED(result))
    {
        return false;
    }

    DXGI_MODE_DESC *displayModeList;
    // Create a list to hold all the possible display modes for this
    // monitor/video card combination.
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
    {
        return false;
    }

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                               DXGI_ENUM_MODES_INTERLACED,
                                               &numModes, displayModeList);
    if (FAILED(result))
    {
        return false;
    }

    unsigned int numerator, denominator;
    for (int i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (unsigned int)appConfig.screenSize[0])
        {
            if (displayModeList[i].Height ==
                (unsigned int)appConfig.screenSize[1])
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    DXGI_ADAPTER_DESC adapterDesc;
    // Get the adapter (video card) description.
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
    {
        return false;
    }

    // Store the dedicated video card memory in megabytes.
    renderState.videoCardMemory =
        (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Convert the name of the video card to a character array and store it.
    unsigned long long stringLength;
    int error = wcstombs_s(&stringLength, renderState.videoCardDescription, 128,
                           adapterDesc.Description, 128);
    if (error != 0)
    {
        return false;
    }

    // Release the display mode list.
    delete[] displayModeList;
    displayModeList = 0;

    // Release the adapter output.
    adapterOutput->Release();
    adapterOutput = 0;

    // Release the adapter.
    adapter->Release();
    adapter = 0;

    // Release the factory.
    factory->Release();
    factory = 0;

    DXGI_MODE_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
    bufferDesc.Width = appConfig.screenSize[0];
    bufferDesc.Height = appConfig.screenSize[1];
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = appState.hWindow;
    swapChainDesc.SampleDesc.Count = 4;
    swapChainDesc.Windowed = !appConfig.fullscreen;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    // Set the refresh rate of the back buffer.
    if (renderConfig.vsync)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the feature level to DirectX 11.
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    // Create the swap chain, Direct3D device, and Direct3D device context.
    result = D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &swapChainDesc, &renderState.swapChain,
        &renderState.device, NULL, &renderState.deviceContext);
    if (FAILED(result))
    {
        return false;
    }

    // Get the pointer to the back buffer.
    ID3D11Texture2D *backBufferPtr;
    result = renderState.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                              (LPVOID *)&backBufferPtr);
    if (FAILED(result))
    {
        return false;
    }

    // Create the render target view with the back buffer pointer.
    result = renderState.device->CreateRenderTargetView(
        backBufferPtr, NULL, &renderState.renderTargetView);
    if (FAILED(result))
    {
        return false;
    }

    // Release pointer to the back buffer as we no longer need it.
    backBufferPtr->Release();
    backBufferPtr = 0;

    // Initialize the description of the depth buffer.
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    // Set up the description of the depth buffer.
    depthBufferDesc.Width = appConfig.screenSize[0];
    depthBufferDesc.Height = appConfig.screenSize[1];
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    result = renderState.device->CreateTexture2D(
        &depthBufferDesc, NULL, &renderState.depthStencilBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Initialize the description of the stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // Create the depth stencil state.

    result = renderState.device->CreateDepthStencilState(
        &depthStencilDesc, &renderState.depthStencilState);
    if (FAILED(result))
    {
        return false;
    }

    // Set the depth stencil state.
    renderState.deviceContext->OMSetDepthStencilState(
        renderState.depthStencilState, 1);

    // Initialize the depth stencil view.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    result = renderState.device->CreateDepthStencilView(
        renderState.depthStencilBuffer, &depthStencilViewDesc,
        &renderState.depthStencilView);
    if (FAILED(result))
    {
        return false;
    }

    // Bind the render target view and depth stencil buffer to the output render
    // pipeline.
    renderState.deviceContext->OMSetRenderTargets(
        1, &renderState.renderTargetView, renderState.depthStencilView);

    // Setup the raster description which will determine how and what polygons
    // will be drawn.
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state from the description we just filled out.
    result = renderState.device->CreateRasterizerState(
        &rasterDesc, &renderState.rasterState);
    if (FAILED(result))
    {
        return false;
    }

    // Now set the rasterizer state.
    renderState.deviceContext->RSSetState(renderState.rasterState);

    // Setup the viewport for rendering.
    renderState.viewport.Width = (float)appConfig.screenSize[0];
    renderState.viewport.Height = (float)appConfig.screenSize[1];
    renderState.viewport.MinDepth = 0.0f;
    renderState.viewport.MaxDepth = 1.0f;
    renderState.viewport.TopLeftX = 0.0f;
    renderState.viewport.TopLeftY = 0.0f;

    // Create the viewport.
    renderState.deviceContext->RSSetViewports(1, &renderState.viewport);

    // Setup the projection matrix.
    float fieldOfView = 3.141592654f / 4.0f;
    float screenAspect =
        (float)appConfig.screenSize[0] / (float)appConfig.screenSize[1];

    // Create the projection matrix for 3D rendering.
    renderState.projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
        fieldOfView, screenAspect, renderConfig.screenRatio[0],
        renderConfig.screenRatio[1]);
    // Initialize the world matrix to the identity matrix.
    renderState.worldMatrix = DirectX::XMMatrixIdentity();

    // Create an orthographic projection matrix for 2D rendering.
    renderState.orthoMatrix = DirectX::XMMatrixOrthographicLH(
        (float)appConfig.screenSize[0], (float)appConfig.screenSize[1],
        renderConfig.screenRatio[0], renderConfig.screenRatio[1]);
    return true;
}

void Renderer_Shutdown()
{
    // Before shutting down set to windowed mode or when you release the swap
    // chain it will throw an exception.
    if (renderState.swapChain)
    {
        renderState.swapChain->SetFullscreenState(false, NULL);
    }

    if (renderState.rasterState)
    {
        renderState.rasterState->Release();
        renderState.rasterState = 0;
    }

    if (renderState.depthStencilView)
    {
        renderState.depthStencilView->Release();
        renderState.depthStencilView = 0;
    }

    if (renderState.depthStencilState)
    {
        renderState.depthStencilState->Release();
        renderState.depthStencilState = 0;
    }

    if (renderState.depthStencilBuffer)
    {
        renderState.depthStencilBuffer->Release();
        renderState.depthStencilBuffer = 0;
    }

    if (renderState.renderTargetView)
    {
        renderState.renderTargetView->Release();
        renderState.renderTargetView = 0;
    }

    if (renderState.deviceContext)
    {
        renderState.deviceContext->Release();
        renderState.deviceContext = 0;
    }

    if (renderState.device)
    {
        renderState.device->Release();
        renderState.device = 0;
    }

    if (renderState.swapChain)
    {
        renderState.swapChain->Release();
        renderState.swapChain = 0;
    }
}

void Renderer_Render()
{
    // early render
    //  Clear the back buffer.
    renderState.deviceContext->ClearRenderTargetView(
        renderState.renderTargetView, renderState.backGroundColor);

    // Clear the depth buffer.
    renderState.deviceContext->ClearDepthStencilView(
        renderState.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // render

    // Late render
    //  Present the back buffer to the screen since rendering is complete.
    renderConfig.vsync
        ? renderState.swapChain->Present(1, 0)
        : renderState.swapChain->Present(
              0,
              0); // Lock to screen refresh rate. : Present as fast as possible.
}

void Renderer_SetupColorBackGround(float color[4])
{
    renderState.backGroundColor[0] = color[0];
    renderState.backGroundColor[1] = color[1];
    renderState.backGroundColor[2] = color[2];
    renderState.backGroundColor[3] = color[3];
}
void Render_SetBackBufferRenderTarget()
{
    renderState.deviceContext->OMSetRenderTargets(
        1, &renderState.renderTargetView, renderState.depthStencilView);
}

void Renderer_ResetViewport()
{
    // Bind the render target view and depth stencil buffer to the output render
    // pipeline.
    renderState.deviceContext->OMSetRenderTargets(
        1, &renderState.renderTargetView, renderState.depthStencilView);
}

CONST_RELEASE RenderState &Renderer_GetState() { return renderState; }

CONST_RELEASE RenderConfig &Renderer_GetConfig() { return renderConfig; }