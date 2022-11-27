// Bradley Christensen - 2022
#include "WindowsApplication.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>






//----------------------------------------------------------------------------------------------------------------------
// WinMain
//
// Simply creates the WindowsApplication and runs it until it decides to quit
//
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
   WindowsApplication& app = WindowsApplication::Get();
   app.Startup();
   app.Run();
   app.Shutdown();
   WindowsApplication::Delete();
   
   return 0;
}