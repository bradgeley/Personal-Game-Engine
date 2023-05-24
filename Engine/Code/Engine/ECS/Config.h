// Bradley Christensen - 2023
#pragma once
#include <stdint.h>



//----------------------------------------------------------------------------------------------------------------------
// Max entities is required for array storage, as they are preallocated
// todo: move to EngineBuildPreferences.h
#ifdef _DEBUG
constexpr uint32_t MAX_ENTITIES = 1'000;
#else
constexpr uint32_t MAX_ENTITIES = 100'000;
#endif


//----------------------------------------------------------------------------------------------------------------------
// Minimum time step. If you try to make system time step lower than this it'll likely cause infinite loops, etc.
constexpr float SYSTEM_MIN_TIME_STEP = 0.001f;


//----------------------------------------------------------------------------------------------------------------------
// Component bit mask, used to keep track of entity composition
typedef size_t BitMask;

//----------------------------------------------------------------------------------------------------------------------
// Component type hash code, taken from typeid(CType).hash_code()
typedef size_t HashCode;


//----------------------------------------------------------------------------------------------------------------------
// Rate at which performance data updates for the System Admin Window graph
constexpr float DATA_CAPTURE_UPDATE_RATE = 1.f;

constexpr uint32_t LAST_ENTITY_INDEX = MAX_ENTITIES - 1;