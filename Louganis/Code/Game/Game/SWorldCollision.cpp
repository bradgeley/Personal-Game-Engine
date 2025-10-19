// Bradley Christensen - 2022-2025
#include "SWorldCollision.h"
#include "CCollision.h"
#include "CTransform.h"
#include "Chunk.h"
#include "SCCollision.h"
#include "SCWorld.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<CCollision, SCWorld, SCCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SWorldCollision::Run(SystemContext const&)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collStorage = g_ecs->GetArrayStorage<CCollision>();
    SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();
    SCCollision const& scCollision = g_ecs->GetSingleton<SCCollision>();

    scWorld.ForEachSolidWorldCoordsOverlappingAABB(scCollision.m_collisionUpdateBounds, [&](WorldCoords const& worldCoords, Chunk& chunk)
    {
        if (!chunk.IsTileSolid(worldCoords.m_localTileCoords))
        {
            return true; // keep iterating
        }

		auto chunkCollisionDataIt = scCollision.m_chunkCollisionData.find(worldCoords.m_chunkCoords);
        if (chunkCollisionDataIt == scCollision.m_chunkCollisionData.end())
        {
			return true; // keep iterating
        }

        ChunkCollisionData const& chunkCollisionData = chunkCollisionDataIt->second;
        auto const& tileBuckets = chunkCollisionData.m_tileBuckets;

		int tileIndex = chunk.m_tiles.GetIndexForCoords(worldCoords.m_localTileCoords);

		auto tileBucketIt = tileBuckets.find(tileIndex);
        if (tileBucketIt == tileBuckets.end())
        {
			return true; // keep iterating
        }

		EntityBucket const& tileBucket = tileBucketIt->second;
		AABB2 tileBounds = scWorld.GetTileBounds(worldCoords);
        
        for (EntityID const& entity : tileBucket)
        {
			CCollision const& collision = collStorage[entity];
            CTransform& transform = transStorage[entity];

			Vec2 newPos = transform.m_pos + collision.m_offset;
			bool collisionResolved = GeometryUtils::PushDiscOutOfAABB2D(newPos, collision.m_radius + 0.0001f, tileBounds);
            if (collisionResolved)
            {
                transform.m_pos = newPos - collision.m_offset;
            }
        }

        return true; // keep iterating
    });
}