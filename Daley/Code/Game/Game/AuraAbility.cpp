// Bradley Christensen - 2022-2026
#include "AuraAbility.h"
#include "AuraAbilityDef.h"
#include "CAbility.h"
#include "CHealth.h"
#include "CTime.h"
#include "CTransform.h"
#include "SCCollision.h"
#include "SCRenderer.h"
#include "SCWorld.h"
#include "DiscShaderCPU.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AuraAbility::AuraAbility(AuraAbilityDef const& def) : Ability(def)
{
	m_radius = def.m_targetingDef.m_maxRange;
	m_targetingComp = def.m_targetingDef;
	m_renderComp = def.m_renderDef;
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::Update(SystemContext const& context, CAbility const&, Vec2 const& location, float timeDilation)
{
	AuraAbilityDef const* def = static_cast<AuraAbilityDef const*>(m_abilityDef);
    ASSERT_OR_DIE(def, "AuraAbility::Update - m_abilityDef is null.");

	// Read Dependencies
	SCWorld const& world = context.GetSingletonConst<SCWorld>();
	SCCollision const& collision = context.GetSingletonConst<SCCollision>();

    // Write Dependencies
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();

	CHealth& ownerHealth = healthStorage[m_owner];
	if (ownerHealth.GetHealthReachedZero())
	{
		return;
	}

	float deltaSeconds = context.m_deltaSeconds * timeDilation;

    bool affectsTowers = def->m_auraType == AuraType::Slow;

    if (affectsTowers)
    {
		world.ForEachPlayableTileOverlappingCircle(location, m_radius, [&](IntVec2 const& coords)
        {
			EntityID entityID = world.GetTileOccupant(coords);
			if (entityID != EntityID::Invalid)
			{
                CTime& time = timeStorage[entityID];
                if (def->m_auraType == AuraType::Slow)
                {
                    time.SetIsSlowedThisFrame(true);
                }
			}
            return true;
        });
    }
    else // Affects enemies
    {
        world.ForEachPathTileOverlappingCircle(location, m_radius, [&](IntVec2 const& coords)
        {
			int tileIndex = world.m_tiles.GetIndexForCoords(coords);
			collision.ForEachEnemyInTile(tileIndex, [&](EntityID entityID)
			{
				switch (def->m_auraType)
				{
                    case AuraType::Protection:
                    {
						CHealth& health = healthStorage[entityID];
                        if (health.GetCannotBeProtected())
                        {
                            break;
                        }
                        health.SetIsProtected(true);
						break;
                    }
                    case AuraType::Haste:
                    {
                        CTime& time = timeStorage[entityID];
                        time.SetIsHastedThisFrame(true);
                        break;
                    }
                    case AuraType::BurnSuppression:
                    {
						CHealth& health = healthStorage[entityID];
						health.SetIsSuppressingBurn(true);
                        break;
                    }
                    case AuraType::PoisonSuppression:
                    {
                        CHealth& health = healthStorage[entityID];
                        health.SetIsSuppressingPoison(true);
                        break;
                    }
                    case AuraType::Healing:
                    {
						CHealth& health = healthStorage[entityID];
                        float healing = StaticGameSettings::s_healingAuraPercentHealthPerSec * health.m_maxHealth * deltaSeconds;
						health.TakeHealing(healing);
						break;
                    }
					default:
						break;
				}
				return true;
			});
            return true;
        });
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const
{
    CHealth& ownerHealth = *context.GetComponent<CHealth>(m_owner);
    if (ownerHealth.GetHealthReachedZero())
    {
        return;
    }

    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);

    DiscRenderInstance discInstance;
    discInstance.m_position = Vec3(location, m_renderComp.m_depth);
    discInstance.m_radius = m_targetingComp.GetMaxRange(ability);
	discInstance.m_tint = m_renderComp.m_tint;
    discIBO.AddInstance(discInstance);
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AuraAbility::DeepCopy() const
{
    AuraAbility* copy = new AuraAbility(*reinterpret_cast<AuraAbilityDef const*>(m_abilityDef));
    *copy = *this;
	return copy;
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::CopyTransientDataTo(Ability&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::AddDebugVerts(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability) const
{
    CTransform const* transform = context.GetComponentUnsafe<CTransform>(m_owner);
    if (transform)
    {
        AddDebugVertsAtLocation(out_vbo, context, ability, transform->m_pos);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::AddDebugVertsAtLocation(VertexBuffer& out_vbo, SystemContext const&, CAbility const& ability, Vec2 const& location) const
{
    float maxRange = m_targetingComp.GetMaxRange(ability);
    if (maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, maxRange, 0.1f, 32, Rgba8::Orange);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);

	m_targetingComp.AppendDebugString(debugContext);
    m_renderComp.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
AuraAbilityDef const* AuraAbility::GetDef() const
{
	return static_cast<AuraAbilityDef const*>(m_abilityDef);
}
