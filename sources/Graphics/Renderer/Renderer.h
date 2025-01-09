#ifndef RENDERER_H
#define RENDERER_H
#include "base.h"
struct RenderConfig;
struct RenderState;
void Renderer_SetConfig(RenderConfig &);
bool Renderer_Initialize();
void Renderer_Render();
void Renderer_Shutdown();
void Renderer_ResetViewport();
void Renderer_SetupColorBackGround(float color[4]);
void Renderer_GetVideoCardInfo(char *cardName, int &memory);
CONST_RELEASE RenderConfig &Renderer_GetConfig();
CONST_RELEASE RenderState  &Renderer_GetState();
#endif