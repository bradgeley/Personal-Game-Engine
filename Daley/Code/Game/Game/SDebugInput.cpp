// Bradley Christensen - 2022-2025
#include "SDebugInput.h"
#include "SCCamera.h"
#include "SCDebug.h"
#include "SCWorld.h"
#include "TileDef.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Startup()
{
	AddWriteAllDependencies();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Run(SystemContext const& context)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();

	// Debug mouse position
	if (g_window->HasFocus())
	{
		Vec2 relMousePos = g_input->GetMouseClientRelativePosition();
		scDebug.m_debugMouseWorldLocation = camera.m_camera.ScreenToWorldOrtho(relMousePos);
		scDebug.m_debugMouseTileCoords = world.GetTileCoordsAtWorldPos(scDebug.m_debugMouseWorldLocation);
	}

	// Debug Tile Placement
	if (g_input->IsKeyDown(KeyCode::Ctrl) && g_input->IsKeyDown('B'))
	{
		Tile* tile = world.GetTileAtWorldPos(scDebug.m_debugMouseWorldLocation);
		
		Tile newTile = TileDef::GetDefaultTile(scDebug.m_debugPlacementTileID);
		world.SetTile(scDebug.m_debugMouseTileCoords, newTile);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugInput::Shutdown()
{

}
