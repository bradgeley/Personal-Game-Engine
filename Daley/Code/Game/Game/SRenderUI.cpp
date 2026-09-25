// Bradley Christensen - 2022-2026
#include "SRenderUI.h"
#include "Ability.h"
#include "AbilityDef.h"
#include "EntityDef.h"
#include "FlavorDef.h"
#include "CPlaceable.h"
#include "SCRenderer.h"
#include "SCInputSystem.h"
#include "SCWorld.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Startup()
{
	AddReadDependencies<CAbility, CPlaceable, CTransform>();
	AddReadDependencies<SCInputSystem, SCRunData, SCWorld>();
	AddWriteDependencies<SCRenderer>();

	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	scRenderer.m_immediateVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Shutdown() const
{
	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	if (scRenderer.m_immediateVBO != RendererUtils::InvalidID)
	{
		scRenderer.GetRenderer()->ReleaseVertexBuffer(scRenderer.m_immediateVBO);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCInputSystem const& scInput = context.GetSingletonConst<SCInputSystem>();
	SCWorld const& scWorld = context.GetSingletonConst<SCWorld>();
	SCRunData const& scRunData = context.GetSingletonConst<SCRunData>();
	
	// Write Dependencies
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();
	VertexBuffer& untexturedVerts = *renderer.GetVertexBuffer(scRenderer.m_immediateVBO);
	untexturedVerts.ClearVerts();

	if (scInput.m_towerPlacementIndex != -1)
	{
		TowerPlacementRequest const& placementInfo = scInput.m_towerPlacementRequest;

		EntityDef const* def = EntityDef::GetEntityDef(placementInfo.m_towerEntityName);
		CPlaceable const& placeable = def->m_placeable.value();
		CTags tagsCopy = def->m_tags.has_value() ? *def->m_tags : CTags();

		std::vector<Ability*> abilities;
		CPlaceable placeableCopy = placeable;
		placeableCopy.m_botLeftTile = scInput.m_towerPlacementRequest.m_botLeftTileCoords;

		if (def->m_ability.has_value())
		{
			for (Ability* ability : def->m_ability->m_abilities)
			{
				abilities.push_back(ability->DeepCopy());
			}
		}

		if (placementInfo.m_flavorName != Name::Invalid)
		{
			tagsCopy.AddTag(placementInfo.m_flavorName);
			FlavorDef const* flavorDef = FlavorDef::GetFlavorDef(placementInfo.m_flavorName);
			ASSERT_OR_DIE(flavorDef, StringUtils::StringF("SRenderUI::Run: FlavorDef not found for flavor: %s", placementInfo.m_flavorName.ToCStr()).c_str());
			for (Name ability : flavorDef->m_abilities)
			{
				if (ability == Name::Invalid)
				{
					continue;
				}
				AbilityDef const* abilityDef = AbilityDef::GetAbilityDef(ability);
				abilities.push_back(abilityDef->MakeAbilityInstance());
			}
		}

		for (auto& ability : abilities)
		{
			// Render range indicators for abilities when in placement mode, with all mods applied for accurate range
			ability->AddDebugVerts(untexturedVerts, placeableCopy, CAbility(), placementInfo.m_worldPos);
		}

		for (auto& ability : abilities)
		{
			ability->Shutdown(context);
			delete ability;
		}

		scWorld.ForEachPlayableTileInRegion(placementInfo.m_botLeftTileCoords, placementInfo.m_topRightTileCoords, [&](IntVec2 const& tileCoords)
		{
			bool isTileValid = scWorld.DoesTileMatchTagQuery(tileCoords, placeable.m_tileTagQuery);
			Rgba8 tileTint = isTileValid ? Rgba8(0, 255, 0, 127) : Rgba8(255, 0, 0, 127);
			tileTint = placementInfo.m_canAfford ? tileTint : Rgba8(255, 255, 0, 127); // Orange if can't afford
			VertexUtils::AddVertsForAABB2(untexturedVerts, scWorld.GetTileBounds(tileCoords), tileTint);
			return true;
		});
	}
	else if (scInput.m_towerUnderCursor != EntityID::Invalid)
	{
		// Render info about tower under cursor

		CPlaceable const* placeable = context.GetComponentConst<CPlaceable>(scInput.m_towerUnderCursor);
		CAbility const* abilityComp = context.GetComponentConst<CAbility>(scInput.m_towerUnderCursor);
		CTransform const* transform = context.GetComponentConst<CTransform>(scInput.m_towerUnderCursor);

		if (placeable && abilityComp && transform)
		{
			for (Ability* const& ability : abilityComp->m_abilities)
			{
				ability->AddDebugVerts(untexturedVerts, *placeable, *abilityComp, transform->m_pos);
			}
		}
	}

	renderer.BindTexture();
	renderer.BindShader();
	renderer.DrawVertexBuffer(scRenderer.m_immediateVBO);
}