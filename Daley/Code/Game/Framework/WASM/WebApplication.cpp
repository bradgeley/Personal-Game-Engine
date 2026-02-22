// Bradley Christensen - 2022-2026
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(__EMSCRIPTEN__)
#include "WebApplication.h"
#include <emscripten.h>




//----------------------------------------------------------------------------------------------------------------------
void WebApplication::Shutdown()
{
    Application::Shutdown();

    emscripten_cancel_main_loop();
}

#endif // __EMSCRIPTEN__