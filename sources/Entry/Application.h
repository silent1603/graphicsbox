#ifndef APPLICATION_H
#define APPLICATION_H
#include "base.h"
struct AppState;
struct AppConfig;
void Application_SetConfig(AppConfig &);
bool Application_Initialize();
void Application_Run();
void Application_Shutdown();
CONST_RELEASE AppState &Application_GetAppState();
CONST_RELEASE AppConfig &Application_GetAppConfig();
#endif