// Bradley Christensen - 2023
#pragma once
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
struct SCDebug
{
	bool m_physicsDebugDraw = false;
	bool m_fpsCounter = true;

	Camera debugCamera;
	VertexBuffer m_debugDrawVerts;
};
