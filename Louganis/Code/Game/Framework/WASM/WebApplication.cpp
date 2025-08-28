// Bradley Christensen - 2022-2025
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(__EMSCRIPTEN__)
#include "WebApplication.h"
#include <emscripten.h>



//----------------------------------------------------------------------------------------------------------------------
void WebApplication::Shutdown()
{
    Application::Shutdown();

    emscripten_cancel_main_loop(0);
}

#endif // __EMSCRIPTEN__