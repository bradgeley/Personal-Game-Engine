// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Math/Vec2.h"



typedef uint8_t TileID;



//----------------------------------------------------------------------------------------------------------------------
class SCDebug
{
public:

	VertexBufferID m_frameUntexVerts		= RendererUtils::InvalidID;
	VertexBufferID m_frameDefaultFontVerts	= RendererUtils::InvalidID;

	// Overlay
	bool m_debugOverlayEnabled				= false;

	// Debug draw toggles
	bool m_debugRenderGrid					= false;
};

