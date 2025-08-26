// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/RendererUtils.h"



//----------------------------------------------------------------------------------------------------------------------
class SCDebug
{
public:

	VertexBufferID m_frameUntexVerts = RendererUtils::InvalidID;
	VertexBufferID m_frameDefaultFontVerts = RendererUtils::InvalidID;
	bool m_debugRenderDistanceField = false;
	bool m_debugRenderToMouseRaycast = false;
	bool m_debugRenderToMouseDiscCast = false;
	bool m_debugRenderCostField = false;
	bool m_debugRenderGradient = false;
	bool m_debugRenderPreventativePhysicsRaycasts = false;
	bool m_debugRenderSolidTiles = false;
	Vec2 m_debugMouseLocation;
};

