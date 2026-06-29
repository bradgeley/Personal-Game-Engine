// Bradley Christensen - 2022-2026
#include "SRenderUI.h"
#include "EntityDef.h"
#include "CPlaceable.h"
#include "SCRenderer.h"
#include "SCInputSystem.h"
#include "SCWorld.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Startup()
{
	AddReadDependencies<SCGame, SCInputSystem, SCWorld>();
	AddWriteDependencies<SCRenderer>();
	
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Shutdown() const
{

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

	if (scInput.m_isInTowerPlacementMode)
	{
		TowerPlacementInfo const& placementInfo = scInput.m_towerPlacementInfo;
		EntityDef const* def = EntityDef::GetEntityDef(placementInfo.m_towerName);
		CPlaceable const& placeable = def->m_placeable.value();

		scWorld.ForEachPlayableTileInRegion(placementInfo.m_botLeftTileCoords, placementInfo.m_topRightTileCoords, [&](IntVec2 const& tileCoords)
		{
			Rgba8 tileTint = scWorld.DoesTileMatchTagQuery(tileCoords, placeable.m_tileTagQuery) ? Rgba8(0, 255, 0, 127) : Rgba8(255, 0, 0, 127);
			VertexUtils::AddVertsForAABB2(untexturedVerts, scWorld.GetTileBounds(tileCoords), tileTint);
			return true;
		});
	}

	renderer.BindTexture();
	renderer.BindShader();
	renderer.DrawVertexBuffer(scRenderer.m_immediateVBO);
}