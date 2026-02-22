// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/TagQuery.h"
#include "Engine/Math/Vec2.h"



class SCWorld;
class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
struct WorldRaycast
{
    WorldRaycast() = default;
    WorldRaycast(Vec2 const& start, Vec2 const& dir, float maxDist) : m_start(start), m_direction(dir), m_maxDistance(maxDist) {}

    bool m_queryWorldTiles = true;
    //bool m_queryActors = false; TODO
    TagQuery m_tileTagQuery;
    Vec2 m_start;
    Vec2 m_direction;
    float m_maxDistance = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct WorldDiscCast : public WorldRaycast
{
    float m_discRadius = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct WorldRaycastResult
{
    WorldRaycast m_raycast;
    bool m_blockingHit = false;
    bool m_immediateHit = false;
    Vec2 m_hitLocation;
    Vec2 m_hitNormal;
    float m_distance = 0.f;
    float m_t = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct WorldDiscCastResult 
{
    WorldDiscCast m_discCast;
    bool m_blockingHit = false;
    bool m_immediateHit = false;
    Vec2 m_hitLocation;
    Vec2 m_hitNormal;
    Vec2 m_newDiscCenter;
    float m_distance = 0.f;
    float m_t = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
WorldRaycastResult Raycast(SCWorld const& world, WorldRaycast const& raycast);
WorldDiscCastResult DiscCast(SCWorld const& world, WorldDiscCast const& discCast);
void DebugDrawRaycast(WorldRaycastResult const& result);
void AddVertsForRaycast(VertexBuffer& vbo, WorldRaycastResult const& result, float scaleMultiplier = 1.f);
void AddVertsForDiscCast(VertexBuffer& vbo, WorldDiscCastResult const& result, float scaleMultiplier = 1.f);