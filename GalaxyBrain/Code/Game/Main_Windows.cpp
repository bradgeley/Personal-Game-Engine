// Bradley Christensen - 2022-2023
#include "Game/WindowsApplication.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>






//----------------------------------------------------------------------------------------------------------------------
// WinMain
//
// Simply creates the WindowsApplication and runs it until it decides to quit
//
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
   g_app = new WindowsApplication();
   g_app->Startup();
   g_app->Run();
   g_app->Shutdown();
   delete g_app;
   g_app = nullptr;
   
   return 0;
}