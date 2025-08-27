// Bradley Christensen - 2022-2025
#pragma once



class Window;
struct WindowConfig;



//----------------------------------------------------------------------------------------------------------------------
namespace WindowUtils
{
	Window* MakeWindow(WindowConfig const& config);
}
