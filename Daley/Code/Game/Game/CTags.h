// Bradley Christensen - 2022-2026
#pragma once
#include "GameCommon.h"
#include "Engine/Core/Name.h"
#include "Engine/Renderer/Rgba8.h"
#include <string>
#include <array>



struct FlavorDef;




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
	std::array<FlavorDef const*, StaticGameSettings::s_absoluteSwirlLimit> GetFlavorDefs() const;
	Rgba8 GetFlavorTint() const;

	void AppendDebugString(std::string& out) const;

	static constexpr int s_maxTags = 8;

private:
    
	int m_tagCount = 0;
    std::array<Name, s_maxTags> m_tags = { Name::Invalid };
};
