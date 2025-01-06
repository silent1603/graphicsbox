#ifndef CORE_H
#define CORE_H

struct AppConfig;
struct AppState;

void Application_SetConfig(AppConfig &);
bool Application_Initialize();
void Application_Run();
void Application_Shutdown();
const AppState &Application_GetAppState();
#endif