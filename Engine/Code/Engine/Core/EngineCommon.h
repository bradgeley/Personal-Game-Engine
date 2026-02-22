// Bradley Christensen - 2022-2026
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// Engine Common
//
// Common macros and functions for use in the engine
//
#define UNUSED(x) (void) (x);
#define SHUTDOWN_AND_DESTROY(x) { if ((x)) { (x)->Shutdown(); delete (x); } (x) = nullptr; }


//----------------------------------------------------------------------------------------------------------------------
// RTTI Detection
//
#if defined(__clang__) || defined(__GNUC__)
	#ifdef __GXX_RTTI
		#define RTTI_ENABLED 1
	#else
		#define RTTI_ENABLED 0
	#endif
#elif defined(_MSC_VER)
	#ifdef _CPPRTTI
		#define RTTI_ENABLED 1
	#else
		#define RTTI_ENABLED 0
	#endif
#else
	#define RTTI_ENABLED 1 // assume RTTI is on
#endif