// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// Engine Build Preferences
//
// #define's used to customize functionality in the engine



//----------------------------------------------------------------------------------------------------------------------
// Performance
//
//#define DISABLE_ASSERTS
//#define PERF_WINDOW_DISPLAY_ENGINE_SECTION
#define PERF_WINDOW_LOG_ENGINE_FRAME_DATA



//----------------------------------------------------------------------------------------------------------------------
// Debugging
//
//#define DEBUG_MEMORY_LEAKS
#if defined(_DEBUG)
	#define NAME_USE_DEBUG_STRING
#endif
#define DEBUG_ASSET_MANAGER



//----------------------------------------------------------------------------------------------------------------------
// Renderer
//
#if defined(_WIN32)
	#define RENDERER_D3D11
#endif



//----------------------------------------------------------------------------------------------------------------------
// Audio System
//
#define AUDIO_SYSTEM_ENABLED
#define AUDIO_SYSTEM_USE_MINI_AUDIO