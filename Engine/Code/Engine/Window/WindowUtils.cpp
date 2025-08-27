// Bradley Christensen - 2022-2025
#include "WindowUtils.h"

#if defined(_WIN32)
#include "Win32/Win32Window.h"
#endif // _WIN32



//----------------------------------------------------------------------------------------------------------------------
Window* WindowUtils::MakeWindow(WindowConfig const& config)
{
#if defined(_WIN32)
    return new Win32Window(config);
#else
	return nullptr;
#endif // _WIN32
}
