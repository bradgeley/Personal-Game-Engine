// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"



typedef uint8_t TileID;



//----------------------------------------------------------------------------------------------------------------------
class SCDebug
{
public:

	// Debug cached info
	Vec2 m_debugMouseWorldLocation			= Vec2::ZeroVector;				// Updated by SDebugInput
	IntVec2 m_debugMouseTileCoords			= IntVec2::ZeroVector;			// Updated by SDebugInput
	VertexBufferID m_frameUntexVerts		= RendererUtils::InvalidID;		// Rendered then cleared each frame by SDebugRender
	VertexBufferID m_frameDefaultFontVerts	= RendererUtils::InvalidID;		// Rendered then cleared each frame by SDebugRender

	// Debug Placement
	TileID m_debugPlacementTileID			= 12;							// Tile to place when using ctrl+B, used by SDebugInput
	Name m_debugPlacementTowerName			= "Vanilla";					// Tower to place when using ctrl+T, used by SDebugInput

	// Overlay
	bool m_debugOverlayEnabled				= false;						// Toggle used by SDebugOverlay

	// Debug draw toggles
	bool m_debugRenderGrid					= false;						// Toggle used by SDebugRender
	bool m_debugRenderEdges					= false;						// Toggle used by SDebugRender
	bool m_debugRenderCostField				= false;						// Toggle used by SDebugRender
	bool m_debugRenderDistanceField			= false;						// Toggle used by SDebugRender
	bool m_debugRenderFlowField				= false;						// Toggle used by SDebugRender
	bool m_debugRenderCollision				= false;						// Toggle used by SDebugRender
	bool m_debugRenderWeapons				= false;						// Toggle used by SDebugRender
	uint8_t m_debugTileTags					= 0;							// Used by SDebugRender to display tiles that match the debug tags requested
};

