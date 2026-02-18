// Bradley Christensen - 2022-2025
#include "Weapon.h"
#include "CProjectile.h"
#include "EntityDef.h"
#include "SCCollision.h"
#include "SCFlowField.h"
#include "SCWorld.h"
#include "SEntityFactory.h"
#include "WeaponDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
ProjectileHitWeapon::ProjectileHitWeapon(ProjectileHitWeaponDef const& def)
{
    m_weaponDef = &def;
    m_projectileDefName = def.m_projectileDefName;
    m_damage = def.m_baseDamage;
    m_attacksPerSecond = def.m_baseAttacksPerSecond;
    m_projSpeedUnitsPerSec = def.m_baseProjSpeedUnitsPerSec;
	m_minRange = def.m_baseMinRange;
	m_maxRange = def.m_baseMaxRange;
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitWeapon::Update(float deltaSeconds, Vec2 const& location)
{
	m_accumulatedAttackTime += deltaSeconds;

	constexpr float maxAttacksPerSecond = 1000.f;

	float attacksPerSecond = m_attacksPerSecond;
    ASSERT_OR_DIE(m_attacksPerSecond >= 0.f, "ProjectileHitWeapon::Update - negative attacks per second.");

    if (attacksPerSecond > maxAttacksPerSecond)
    {
        attacksPerSecond = maxAttacksPerSecond;
	}
	float timeBetweenAttacks = 1.f / attacksPerSecond;
    ASSERT_OR_DIE(timeBetweenAttacks > 0.f, "ProjectileHitWeapon::Update - time between attacks is 0 or negative.");

    // Find targets
    SCCollision const& collision = g_ecs->GetSingleton<SCCollision>();
    SCFlowField const& flowfield = g_ecs->GetSingleton<SCFlowField>();
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();

	EntityID targetID = EntityID::Invalid;
    if (m_targetingMode == WeaponTargetingMode::ClosestToGoal)
    {
		float closestToGoalDist = FLT_MAX;
		IntVec2 closestToGoalCoords = IntVec2::ZeroVector;
        world.ForEachPathTileInRange(location, m_minRange, m_maxRange, [&](IntVec2 const& worldCoords)
        {
			int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
			float distance = flowfield.m_toGoalFlowField.m_distanceField.Get(tileIndex);
            if (distance < closestToGoalDist && !collision.m_tileBuckets[tileIndex].empty()) // todo: only check "Enemy" collision layer
            {
				closestToGoalCoords = worldCoords;
				closestToGoalDist = distance;
				targetID = collision.m_tileBuckets[tileIndex].front();
            }
            return true;
        });
    }

    if (targetID != EntityID::Invalid)
    {
        // Shoot at targets
        while (m_accumulatedAttackTime > timeBetweenAttacks)
        {
            m_accumulatedAttackTime -= timeBetweenAttacks;

            SpawnInfo spawnInfo;
            spawnInfo.m_spawnPos = location;
            spawnInfo.m_spawnOrientation = 0.f;
            spawnInfo.m_def = EntityDef::GetEntityDef(m_projectileDefName);
            EntityID projectileID = SEntityFactory::SpawnEntity(spawnInfo);
            if (!g_ecs->IsValid(projectileID))
            {
                break;
            }
			CProjectile& projComp = *g_ecs->GetComponent<CProjectile>(projectileID);
			projComp.m_targetID = targetID;
            projComp.m_accumulatedTime += m_accumulatedAttackTime;
			projComp.m_damage = m_damage;
			projComp.m_projSpeedUnitsPerSec = m_projSpeedUnitsPerSec;
        }
    }
    else
    {
        m_accumulatedAttackTime = MathUtils::Clamp(m_accumulatedAttackTime, 0.f, timeBetweenAttacks);
    }
}



//----------------------------------------------------------------------------------------------------------------------
Weapon* ProjectileHitWeapon::DeepCopy() const
{
    ProjectileHitWeapon* copy = new ProjectileHitWeapon(*reinterpret_cast<ProjectileHitWeaponDef const*>(m_weaponDef));
    *copy = *this;
    return copy;
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitWeapon::AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const
{
    if (m_minRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, m_minRange, 0.1f, 32, Rgba8::Green);
    }
    if (m_maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, m_maxRange, 0.1f, 32, Rgba8::Orange);
    }
}
