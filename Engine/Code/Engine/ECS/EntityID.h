// Bradley Christensen - 2023
#pragma once
#include <cstdint>
#include "Config.h"



//----------------------------------------------------------------------------------------------------------------------
// EntityID
//
using EntityID = uint32_t;

constexpr EntityID ENTITY_ID_SINGLETON	= 0;
constexpr EntityID ENTITY_ID_INVALID	= MAX_ENTITIES;
