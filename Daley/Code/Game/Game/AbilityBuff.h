// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityAttributes.h"
#include "Engine/Core/Name.h"



//----------------------------------------------------------------------------------------------------------------------
struct Buff
{
	Name m_name;
	float m_remainingDuration = 0.f; // -1 means infinite duration
	AbilityAttributes m_modifiers;
};