// Bradley Christensen - 2022-2026
#pragma once
#include <cstddef>
#include <stdint.h>



//----------------------------------------------------------------------------------------------------------------------
// Max entities is required for array storage, as they are preallocated
// todo: move to EngineBuildPreferences.h
#ifdef _DEBUG
constexpr uint32_t MAX_ENTITIES = 10'000;
#else
constexpr uint32_t MAX_ENTITIES = 200'000;
#endif



//----------------------------------------------------------------------------------------------------------------------
// Minimum time step. If you try to make system time step lower than this it'll likely cause infinite loops, etc.
constexpr float SYSTEM_MIN_TIME_STEP = 0.001f;



//----------------------------------------------------------------------------------------------------------------------
// Component bit mask, used to keep track of entity composition
typedef size_t BitMask;