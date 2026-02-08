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
	TileID m_debugPlacementTileID			= 1;							// Tile to place when using ctrl+B, used by SDebugInput

	// Overlay
	bool m_debugOverlayEnabled				= false;						// Toggle used by SDebugOverlay

	// Debug draw toggles
	bool m_debugRenderGrid					= false;						// Toggle used by SDebugRender
};

