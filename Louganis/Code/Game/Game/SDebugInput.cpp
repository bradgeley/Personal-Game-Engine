// Bradley Christensen - 2022-2025
#include "SDebugInput.h"
#include "Chunk.h"
#include "CPlayerController.h"
#include "CMovement.h"
#include "CTransform.h"
#include "SCCamera.h"
#include "SCWorld.h"
#include "SCDebug.h"
#include "TileDef.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Startup()
{
    AddWriteDependencies<SCCamera, CMovement>();
	AddReadDependencies<InputSystem, SCWorld, SCDebug, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Run(SystemContext const& context)
{
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
	SCDebug const& debug = g_ecs->GetSingleton<SCDebug>();
	SCWorld const& world = g_ecs->GetSingleton<SCWorld>();
	auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();

	// Max/Min Zoom Toggle
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

	// Debug Tile Placement
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

	// Teleportation
	for (auto it = g_ecs->Iterate<CMovement, CTransform, CPlayerController>(context); it.IsValid(); ++it)
	{
		CMovement& move = moveStorage[it];
		CTransform const& transform = transformStorage[it];

		if (g_input->IsKeyDown(KeyCode::Ctrl) && g_input->WasKeyJustPressed('T'))
		{
			Vec2 relMousePos = g_input->GetMouseClientRelativePosition();
			Vec2 worldMousePos = camera.m_camera.ScreenToWorldOrtho(relMousePos);
			move.m_frameMovement = worldMousePos - transform.m_pos;
			move.SetIsTeleporting(true);
		}
		else
		{
			move.SetIsTeleporting(false);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Shutdown()
{

}
