// Bradley Christensen - 2022-2026
#pragma once
#include "GameCommon.h"
#include "Engine/Core/Name.h"
#include "Engine/Renderer/Rgba8.h"
#include <string>
#include <array>



struct FlavorDef;



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_TAGS = 8;



//----------------------------------------------------------------------------------------------------------------------
struct CTags
{
public:

    CTags() = default;
    explicit CTags(void const* xmlElement);

	bool AddTag(Name const& tag);
	bool RemoveTag(Name const& tag);
	bool HasTag(Name const& tag) const;
	int FindTag(Name const& tag) const; // returns the index of the tag

	// Utility funcs
	int GetNumFlavorTags() const;
	std::array<FlavorDef const*, StaticGameSettings::s_maxFlavorsInOneTower> GetFlavorDefs() const;
	Rgba8 GetFlavorTint() const;

	void AppendDebugString(std::string& out) const;

private:
    
    std::array<Name, MAX_TAGS> m_tags = { Name::Invalid };
};
