// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// Engine Common
//
// Common macros and functions for use in the engine
//
#define UNUSED(x) (void) (x);
#define SHUTDOWN_AND_DESTROY(x) { if ((x)) { (x)->Shutdown(); delete (x); } (x) = nullptr; }
