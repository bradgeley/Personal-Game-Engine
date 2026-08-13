// Bradley Christensen - 2022-2026
#include "SRenderUI.h"
#include "EntityDef.h"
#include "CPlaceable.h"
#include "SCRenderer.h"
#include "SCInputSystem.h"
#include "SCWorld.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Startup()
{
	AddReadDependencies<SCInputSystem, SCWorld>();
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
	
	// Write Dependencies
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();
	VertexBuffer& untexturedVerts = *renderer.GetVertexBuffer(scRenderer.m_immediateVBO);
	untexturedVerts.ClearVerts();

	if (scInput.m_towerPlacementIndex != -1)
	{
		TowerPlacementRequest const& placementInfo = scInput.m_towerPlacementRequest;
		EntityDef const* def = EntityDef::GetEntityDef(placementInfo.m_towerName);
		CPlaceable const& placeable = def->m_placeable.value();

		scWorld.ForEachPlayableTileInRegion(placementInfo.m_botLeftTileCoords, placementInfo.m_topRightTileCoords, [&](IntVec2 const& tileCoords)
		{
			bool isTileValid = scWorld.DoesTileMatchTagQuery(tileCoords, placeable.m_tileTagQuery);
			Rgba8 tileTint = isTileValid ? Rgba8(0, 255, 0, 127) : Rgba8(255, 0, 0, 127);
			tileTint = placementInfo.m_canAfford ? tileTint : Rgba8(255, 255, 0, 127); // Orange if can't afford
			VertexUtils::AddVertsForAABB2(untexturedVerts, scWorld.GetTileBounds(tileCoords), tileTint);
			return true;
		});
	}

	renderer.BindTexture();
	renderer.BindShader();
	renderer.DrawVertexBuffer(scRenderer.m_immediateVBO);
}