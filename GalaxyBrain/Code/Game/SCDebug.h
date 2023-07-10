// Bradley Christensen - 2023
#pragma once
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
struct SCDebug
{
	bool m_physicsDebugDraw = false;
	bool m_fpsCounter = true;

	VertexBuffer m_debugDrawVerts;
};
