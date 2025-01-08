#include "Render/DirectX/D3D11_Renderer.h"
#include "Entry/Windows/Application_Win_Impl.h"


local_variable RenderState renderState;
local_variable RenderConfig renderConfig;

void Renderer_SetConfig(RenderConfig &config) { renderConfig = config; }

bool Renderer_Initialize()
{
    const AppState& appState = Application_GetAppState();
    const AppConfig& appConfig = Application_GetAppConfig();
    
    HRESULT result;

    // Create a DirectX graphics interface factory.
    IDXGIFactory* factory;
    result = CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)&factory);
    if(FAILED(result))
    {
        return false;
    }

    // Use the factory to create an adapter for the primary graphics interface (video card).
    IDXGIAdapter* adapter;
    result = factory->EnumAdapters(0,&adapter);
    {
        return false;
    }

    // Enumerate the primary adapter output (monitor).
    IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return false;
	}

    unsigned int numModes;
    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return false;
	}

    DXGI_MODE_DESC* displayModeList;
    // Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

    unsigned int numerator, denominator;
	for(int i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)appConfig.screenSize[0])
		{
			if(displayModeList[i].Height == (unsigned int)appConfig.screenSize[1])
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	DXGI_ADAPTER_DESC adapterDesc;
	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

    DXGI_MODE_DESC bufferDesc;
    ZeroMemory(&bufferDesc,sizeof(DXGI_MODE_DESC));
    bufferDesc.Width = appConfig.screenSize[0];
    bufferDesc.Height = appConfig.screenSize[1];
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd,sizeof(DXGI_SWAP_CHAIN_DESC));
    scd.BufferDesc = bufferDesc;
    scd.BufferCount = 1;
    scd.SampleDesc.Quality = 0;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = appState.hWindow;
    scd.SampleDesc.Count = 4;
    scd.Windowed = !appConfig.fullscreen;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


}

void Renderer_Shutdown() 
{
    renderState.swapChain->Release();
    renderState.device->Release();
    renderState.deviceContext->Release();
}

void Renderer_SetBackBufferRenderTarget() {}

void Renderer_ResetViewport() {}
CONST_RELEASE RenderState &Renderer_GetState() {}