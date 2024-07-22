// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/Vec2.h"



class SCWorld;



//----------------------------------------------------------------------------------------------------------------------
struct WorldRaycast
{
    WorldRaycast() = default;
    WorldRaycast(Vec2 const& start, Vec2 const& dir, float maxDist) : m_start(start), m_direction(dir), m_maxDistance(maxDist) {}

    bool m_queryWorldTiles = true;
    //bool m_queryActors = false; TODO
    Vec2 m_start;
    Vec2 m_direction;
    float m_maxDistance = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct WorldRaycastResult
{
    WorldRaycast m_raycast;
    bool m_blockingHit = false;
    bool m_immediateHit = false;
    Vec2 m_hitLocation;
    Vec2 m_hitNormal;
};



//----------------------------------------------------------------------------------------------------------------------
WorldRaycastResult Raycast(SCWorld const& world, WorldRaycast const& raycast);
void DebugDrawRaycast(WorldRaycastResult const& result);