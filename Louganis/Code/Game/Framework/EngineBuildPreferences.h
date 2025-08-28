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
//#define DEBUG_MEMORY_LEAKS // just does a dump at the end
//#define PERF_WINDOW_DISPLAY_ENGINE_SECTION
#define PERF_WINDOW_LOG_ENGINE_FRAME_DATA



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