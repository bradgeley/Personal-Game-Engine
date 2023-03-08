// Bradley Christensen - 2022-2023
#include "Game/Game/WindowsApplication.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>






//----------------------------------------------------------------------------------------------------------------------
// WinMain
//
// Simply creates the WindowsApplication and runs it until it decides to quit
//
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
   g_theApp = new WindowsApplication();
   g_theApp->Startup();
   g_theApp->Run();
   g_theApp->Shutdown();
   delete g_theApp;
   g_theApp = nullptr;
   
   return 0;
}