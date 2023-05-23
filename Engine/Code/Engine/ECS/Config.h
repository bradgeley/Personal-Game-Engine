// Bradley Christensen - 2023
#pragma once
#include <stdint.h>



//----------------------------------------------------------------------------------------------------------------------
// Max entities is required for array storage, as they are preallocated
#ifdef _DEBUG
constexpr uint32_t MAX_ENTITIES = 1000;
#else
constexpr uint32_t MAX_ENTITIES = 200'000;
#endif


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