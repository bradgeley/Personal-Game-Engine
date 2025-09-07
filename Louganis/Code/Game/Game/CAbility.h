// Bradley Christensen - 2022-2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
struct CAbility
{
	CAbility() = default;
	CAbility(void const* xmlElement);

	bool m_isCastingAbility = false;
	bool m_wasCastingAbilityLastFrame = false;
};
