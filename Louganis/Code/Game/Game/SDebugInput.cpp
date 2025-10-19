// Bradley Christensen - 2022-2025
#include "SDebugInput.h"
#include "Chunk.h"
#include "SCCamera.h"
#include "SCWorld.h"
#include "SCDebug.h"
#include "TileDef.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Startup()
{
    AddWriteDependencies<InputSystem, SCCamera>();
	AddReadDependencies<SCWorld, SCDebug>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Run(SystemContext const&)
{
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
	SCDebug const& debug = g_ecs->GetSingleton<SCDebug>();
	SCWorld const& world = g_ecs->GetSingleton<SCWorld>();

	if (g_input->WasKeyJustPressed('Z'))
	{
		if (camera.m_zoomAmount == camera.m_maxZoom)
		{
			camera.m_zoomAmount = camera.m_minZoom;
		}
		else
		{
			camera.m_zoomAmount = camera.m_maxZoom;
		}
	}

	if (g_input->IsKeyDown(KeyCode::Ctrl) && g_input->IsKeyDown('B'))
	{
		Chunk* chunk = world.GetActiveChunk(debug.m_debugMouseWorldCoords);
		TileDef const* tileDef = TileDef::GetTileDef(debug.m_debugPlacementTileID);
		if (tileDef)
		{
			Tile tile = TileDef::GetDefaultTile(debug.m_debugPlacementTileID);
			tile.m_staticLighting = 255;
			chunk->SetTile(debug.m_debugMouseWorldCoords.m_localTileCoords, tile);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Shutdown()
{

}
