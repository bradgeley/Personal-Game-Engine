// Bradley Christensen - 2022-2026
#include "LaserAbility.h"
#include "CTime.h"
#include "EntityDef.h"
#include "LaserAbilityDef.h"
#include "SEntityFactory.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
LaserAbility::LaserAbility(LaserAbilityDef const& def) : Ability(def)
{
    m_targetingComp = def.m_targetingDef;
	m_onHitComp = def.m_onHitDef;
	m_renderComp = def.m_renderDef;
	m_chainComp = def.m_chainDef;
	m_multishotComp = def.m_multishotDef;
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation)
{
	ASSERT_OR_DIE(m_abilityDef, "LaserAbility::Update - m_abilityDef is null.");

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
    if (deltaSeconds == 0.f)
    {
        return;
    }

	SCWorld& world = context.GetSingleton<SCWorld>();
    auto& enemyLayer = context.GetSingletonConst<SCCollision>().GetCollisionLayer(CollisionChannel::Enemy);
    BitMask healthBit = context.GetComponentBitMask<CHealth>();
    BitMask timeBit = context.GetComponentBitMask<CTime>();

	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();
	auto& transformStorage = context.GetArrayStorage<CTransform>();
	auto& collisionStorage = context.GetArrayStorage<CCollision>();
	auto& collisionEffectStorage = context.GetArrayStorage<CCollisionEffect>();
     
    // Cache tiles in range as optimization, so we never search non path tiles that are out of range
    m_targetingComp.UpdateCachedTiles(context, ability, location);

    int maxTargets = m_multishotComp.GetAdditionalTargets(ability) + 1;
    int maxChains = m_chainComp.GetMaxChains(ability);
    float chainDistance = m_chainComp.GetChainDistance(ability);

    if (!m_targetingComp.FindTargets(context, maxTargets, maxChains, chainDistance))
    {
        return;
    }

    RolledOnHitComponent const payload = RollDamageAndEffects(ability, deltaSeconds);
	if (!payload.IsRelevant())
	{
		return;
	}

    for (int targetIndex = 0; targetIndex < m_targetingComp.m_targetChains.GetDimensions().x; ++targetIndex)
    {
        float chainPayloadMulti = 1.f;

        for (int chainIndex = 0; chainIndex < m_targetingComp.m_targetChains.GetDimensions().y; ++chainIndex)
        {
            EntityID target = m_targetingComp.m_targetChains.Get(IntVec2(targetIndex, chainIndex));
            if (target == EntityID::Invalid)
            {
                break;
            }

			HitPayload chainPayload = payload.m_payload;
            chainPayload *= chainPayloadMulti;

            chainPayloadMulti *= m_chainComp.m_chainPayloadMulti;

            if (chainPayload.IsRelevantToHealth())
            {
                CHealth& healthComp = healthStorage[target];
                healthComp.TakePayload(chainPayload);
            }

            if (chainPayload.IsRelevantToTime())
            {
                CTime& timeComp = timeStorage[target];
				timeComp.TakePayload(chainPayload);
            }

            uint32_t splashCounter = Ability::GetNextSplashId();

            if (payload.m_aoeHitOnHit.IsRelevant())
            {
                CTransform const& splashOrigin = transformStorage[target];
				float splashRadius = payload.m_aoeHitOnHit.m_radius;

                world.ForEachPathTileOverlappingCircle(splashOrigin.m_pos, splashRadius, [&](IntVec2 const& worldCoords)
                {
                    int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);

                    CollisionBucket const& tileBucket = enemyLayer[tileIndex];

                    for (EntityID splashEntityID : tileBucket)
                    {
                        CHealth& healthComp = healthStorage[splashEntityID];
                        if (healthComp.m_lastProcessedBy == splashCounter)
                        {
                            // This targeting component has already processed this entity, skip
                            continue;
                        }

                        healthComp.m_lastProcessedBy = splashCounter;

                        if (!healthComp.GetIsTargetable() || healthComp.GetHealthReachedZero())
                        {
                            continue;
                        }

                        CTransform const& transform = transformStorage[splashEntityID];
                        CCollision const& collision = collisionStorage[splashEntityID];
                        float range = collision.m_radius + splashRadius; // Add target's radius to the range
                        float rangeSquared = range * range;
                        float distSquared = MathUtils::GetDistanceSquared2D(transform.m_pos, splashOrigin.m_pos);
                        if (distSquared > rangeSquared)
                        {
                            continue;
                        }

						if (payload.m_aoeHitOnHit.m_payload.IsRelevantToHealth())
						{
							healthComp.TakePayload(payload.m_aoeHitOnHit.m_payload);
						}
						if (payload.m_aoeHitOnHit.m_payload.IsRelevantToTime())
						{
							CTime& timeComp = timeStorage[splashEntityID];
							timeComp.TakePayload(payload.m_aoeHitOnHit.m_payload);
						}
                    }
                    return true;
				});
            }
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const
{
    // TODO: Add to a vbo and render all lasers at once, to improve laser spam perf

	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

    VertexBuffer& vbo = *renderer.GetVertexBuffer(scRenderer.m_immediateVBO);
    vbo.ClearVerts();

    for (int targetIndex = 0; targetIndex < m_targetingComp.m_targetChains.GetDimensions().x; ++targetIndex)
    {
        Vec2 currentChainStartLocation = location;

        for (int chainIndex = 0; chainIndex < m_targetingComp.m_targetChains.GetDimensions().y; ++chainIndex)
        {
            EntityID target = m_targetingComp.m_targetChains.Get(IntVec2(targetIndex, chainIndex));
            if (!context.IsValid(target))
            {
                continue;
            }

            if (CTransform const* transform = context.GetComponentUnsafe<CTransform>(target))
            {
                VertexUtils::AddVertsForLine2D(vbo, currentChainStartLocation, transform->m_pos, 0.25f, m_renderComp.m_tint, m_renderComp.m_depth);
                currentChainStartLocation = transform->m_pos;
            }
        }
    }

    renderer.BindShader();
    renderer.BindTexture();
    renderer.DrawVertexBuffer(vbo);
}



//----------------------------------------------------------------------------------------------------------------------
Ability* LaserAbility::DeepCopy() const
{
    LaserAbility* copy = new LaserAbility(*reinterpret_cast<LaserAbilityDef const*>(m_abilityDef));
    *copy = *this;
	return copy;
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::CopyTransientDataTo(Ability&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const&, CAbility const& ability, Vec2 const& location) const
{
    float maxRange = m_targetingComp.GetMaxRange(ability);
    if (maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, maxRange, 0.1f, 32, Rgba8::Orange);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);

	m_cooldownComp.AppendDebugString(debugContext);
	m_critComp.AppendDebugString(debugContext);
    m_targetingComp.AppendDebugString(debugContext);    
    m_onHitComp.AppendDebugString(debugContext);
	m_chainComp.AppendDebugString(debugContext);
	m_multishotComp.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
RolledOnHitComponent LaserAbility::RollDamageAndEffects(CAbility const& ability, float deltaSeconds) const
{
	RolledOnHitComponent rolledHitResult;

    if (ability.m_abilityFlags.HasFlag(AbilityFlag::Hit) && m_onHitComp.m_scaling.IsRelevant())
    {
        rolledHitResult.m_payload = m_onHitComp.m_scaling.CalculateDotPayload(ability, deltaSeconds);
    }

    bool canSplash = ability.m_abilityFlags.HasFlag(AbilityFlag::Splash) || ability.m_abilityFlags.HasFlag(AbilityFlag::Puddle);
    if (canSplash && m_onHitComp.m_aoeHitOnHit.IsRelevant())
    {
        rolledHitResult.m_aoeHitOnHit = m_onHitComp.m_aoeHitOnHit.CalculateDotPayload(ability, deltaSeconds);
    }

	return rolledHitResult;
}