// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Rgba8.h"
#include <array>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_FLAVOR_ABILITIES = 4;



//----------------------------------------------------------------------------------------------------------------------
struct FlavorDef
{
public:

	explicit FlavorDef(XmlElement const* flavorDefXmlElement);

	static void LoadFromXML();
	static void Shutdown();
	static FlavorDef const* GetFlavorDef(Name name);

private:

	static std::vector<FlavorDef> s_flavorDefs;

public:

	Name		m_name				= "Unnamed FlavorDef";
	std::array<Name, MAX_FLAVOR_ABILITIES> m_abilities;

	// Display Data
	Name		m_displayName		= "Unnamed FlavorDef";
	Name		m_spriteSheetName	= Name::Invalid;
	Name		m_animName			= Name::Invalid;
	Rgba8		m_tint				= Rgba8::White;
	std::string m_desc				= "";
};