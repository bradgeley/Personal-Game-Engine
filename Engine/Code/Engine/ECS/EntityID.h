// Bradley Christensen - 2022-2025
#pragma once
#include <cstddef>
#include <cstdint>
#include "Config.h"



//----------------------------------------------------------------------------------------------------------------------
// EntityID
//
using EntityID = uint32_t;

constexpr EntityID ENTITY_ID_SINGLETON	= 0;
constexpr EntityID ENTITY_ID_INVALID	= MAX_ENTITIES;
