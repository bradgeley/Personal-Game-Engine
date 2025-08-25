// Bradley Christensen - 2024
#pragma once



class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
class SCDebug
{
public:

	VertexBuffer* m_frameVerts; // Cleared out each frame
	bool m_debugRenderDistanceField = false;
	bool m_debugRenderToMouseRaycast = false;
	bool m_debugRenderToMouseDiscCast = false;
	bool m_debugRenderCostField = false;
	bool m_debugRenderGradient = false;
	bool m_debugRenderPreventativePhysicsRaycasts = false;
	Vec2 m_debugMouseLocation;
};

