#include "Graphics/Renderer/Renderer.h"
local_variable RenderState  renderState;
local_variable RenderConfig renderConfig;
void Renderer_SetConfig(RenderConfig &config) { renderConfig = config; }
CONST_RELEASE RenderState &Renderer_GetState() { return renderState; }
CONST_RELEASE RenderConfig &Renderer_GetConfig() { return renderConfig; }