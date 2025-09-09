// Bradley Christensen - 2022-2025
#include "SWorldCollision.h"
#include "CCollision.h"
#include "CTransform.h"
#include "Chunk.h"
#include "SCCollision.h"
#include "SCDebug.h"
#include "SCWorld.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/GeometryUtils.h"
#include "WorldRaycast.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<CCollision, SCWorld, SCCollision>();
    AddWriteDependencies<SCDebug>();
}



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();
    auto& scWorld = g_ecs->GetSingleton<SCWorld>();
	auto& scCollision = g_ecs->GetSingleton<SCCollision>();

    scWorld.ForEachSolidWorldCoordsOverlappingAABB(scCollision.m_collisionUpdateBounds, [&](WorldCoords const& worldCoords, Chunk* chunk)
    {
        if (!chunk->IsTileSolid(worldCoords.m_localTileCoords))
        {
            return true; // keep iterating
        }

        ChunkCollisionData& chunkCollisionData = scCollision.m_chunkCollisionData[worldCoords.m_chunkCoords];
        auto& tileBuckets = chunkCollisionData.m_tileBuckets;
		auto& tileBucket = tileBuckets[chunk->m_tileIDs.GetIndexForCoords(worldCoords.m_localTileCoords)];
		AABB2 tileBounds = scWorld.GetTileBounds(worldCoords);
        
        for (auto entity : tileBucket)
        {
			GeometryUtils::PushDiscOutOfAABB2D(transStorage[entity].m_pos, collStorage[entity].m_radius, tileBounds);
        }

        return true; // keep iterating
    });
}