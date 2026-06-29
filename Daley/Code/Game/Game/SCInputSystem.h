// Bradley Christensen - 2022-2025
#pragma once
#include "TowerPlacementInfo.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"



class InputSystem;



//----------------------------------------------------------------------------------------------------------------------
class SCInputSystem
{
public:

	InputSystem const* GetInputSystem() const		{ return m_inputSystem; }
	InputSystem* GetInputSystem()					{ return m_inputSystem; }
	void SetInputSystem(InputSystem* inputSystem)	{ m_inputSystem = inputSystem; }

public:

	Vec2 m_mouseWorldLocation = Vec2::ZeroVector;				// Updated by SInput
	Vec2 m_mouseViewportRelativePos = Vec2::ZeroVector;			// Updated by SInput
	IntVec2 m_mouseTileCoords = IntVec2::ZeroVector;			// Updated by SInput
	IntVec2 m_mouseIntersectionCoords = IntVec2::ZeroVector;	// Updated by SInput

	bool m_isInTowerPlacementMode = false;
	TowerPlacementInfo m_towerPlacementInfo;

private:

	InputSystem* m_inputSystem = nullptr;
};