// Bradley Christensen - 2022-2025
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include "Game/Game/Application.h"



//----------------------------------------------------------------------------------------------------------------------
void MainGameLoop();



//----------------------------------------------------------------------------------------------------------------------
// main
//
// Simply creates the Application and runs it until it decides to quit
//
int main()
{

	g_app = new Application();
	g_app->Startup();
	empscripten_set_main_loop(MainGameLoop, 0, true);
	g_app->Shutdown();
	delete g_app;
	g_app = nullptr;
	return 0;
}


void MainGameLoop() 
{
	if (g_app != nullptr)
	{
		g_app->Run();
	}	
}

#endif // __EMSCRIPTEN__