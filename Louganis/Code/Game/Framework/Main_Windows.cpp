// Bradley Christensen - 2022-2025
#include "Game/Game/WindowsApplication.h"
#include "EngineBuildPreferences.h"


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


#if defined(DEBUG_MEMORY_LEAKS)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif



//----------------------------------------------------------------------------------------------------------------------
// WinMain
//
// Simply creates the WindowsApplication and runs it until it decides to quit
//
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	#if defined(DEBUG_MEMORY_LEAKS)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(8240); // Specific memory leak
	#endif 

	g_app = new WindowsApplication();
	g_app->Startup();
	g_app->Run();
	g_app->Shutdown();
	delete g_app;
	g_app = nullptr;

	#if defined(DEBUG_MEMORY_LEAKS)
		_CrtDumpMemoryLeaks(); // Note: This catches some things that aren't really a huge problem, like static std::vector's that haven't been cleared out, etc.
	#endif
	return 0;
}