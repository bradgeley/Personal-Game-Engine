// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
struct RunModifierDef
{
public:

	static RunModifierDef const* MakeFromXml(XmlElement const& element);
};



//----------------------------------------------------------------------------------------------------------------------
struct TowerRunModifierDef : public RunModifierDef
{
public:

	TowerRunModifierDef(XmlElement const& element);

	// Allows placement of this tower
	Name m_towerName = Name::Invalid;

	char m_defaultKey = '1';

	float m_baseCost = 1.f;
};