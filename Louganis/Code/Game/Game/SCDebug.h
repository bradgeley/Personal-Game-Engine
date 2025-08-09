// Bradley Christensen - 2024
#pragma once
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
class SCDebug
{
public:

	// Cleared out each frame
	VertexBuffer FrameVerts;
	bool m_debugRenderFlowField = false;
	bool m_debugRenderToMouseRaycast = false;
	bool m_debugRenderCostField = false;
	bool m_debugRenderGradient = false;
	bool m_debugRenderPreventativePhysicsRaycasts = false;
};

