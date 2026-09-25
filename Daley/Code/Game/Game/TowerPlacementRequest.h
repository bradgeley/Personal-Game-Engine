// Bradley Christensen - 2022-2026
#pragma once
#include "GameCommon.h"
#include "Engine/ECS/EntityID.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/TagQuery.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct TowerPlacementRequest
{
	Name m_towerEntityName = "Tower2x2"; // Only type of tower for now
	Name m_flavorName = Name::Invalid;
	bool m_isGenerated = false;

	// Placement Info
	Vec2 m_worldPos = Vec2::ZeroVector;
	IntVec2 m_botLeftTileCoords = IntVec2::ZeroVector;
	IntVec2 m_topRightTileCoords = IntVec2::ZeroVector;
	IntVec2 m_dims = IntVec2::OneVector;
	TagQuery m_tileTagQuery;

	// Cost info
	float m_cost = 0.f;
	bool m_canAfford = true;
};



//----------------------------------------------------------------------------------------------------------------------
struct TowerSwirlRequest
{
	EntityID m_towerEntityID	= EntityID::Invalid;
	Name m_flavor				= Name::Invalid;
	float m_cost				= StaticGameSettings::s_baseSwirlCost;
	bool m_canAfford			= true;
	Vec2 m_worldPos				= Vec2::ZeroVector;
};



//----------------------------------------------------------------------------------------------------------------------
struct TowerRemovalRequest
{
	EntityID m_towerEntityID = EntityID::Invalid;
	Vec2 m_worldPos = Vec2::ZeroVector;
	bool m_isSell = false;
};