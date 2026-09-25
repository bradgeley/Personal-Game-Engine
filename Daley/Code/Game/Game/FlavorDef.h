// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityAttributes.h"
#include "AbilityFlags.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Rgba8.h"
#include <array>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct FlavorDef
{
public:

	explicit FlavorDef(XmlElement const* flavorDefXmlElement);

	static void LoadFromXML();
	static void Shutdown();
	static FlavorDef const* GetFlavorDef(Name name);
	static std::vector<FlavorDef> const& GetAllFlavorDefs();

private:

	static std::vector<FlavorDef> s_flavorDefs;
	static constexpr int s_maxAbilities = 4;

public:

	Name m_name	= "Unnamed FlavorDef";
	std::array<Name, s_maxAbilities> m_abilities;
	AbilityAttributes m_attributes;
	AbilityFlags m_abilityFlags;

	// Display Data
	Name m_displayName		= "Unnamed FlavorDef";
	Name m_spriteSheetName	= Name::Invalid;
	Name m_animName			= Name::Invalid;
	Rgba8 m_tint			= Rgba8::White;
	std::string m_desc		= "";
};