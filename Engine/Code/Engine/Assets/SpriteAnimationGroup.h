// Bradley Christensen - 2022-2026
#pragma once
#include "SpriteAnimation.h"



struct Vec2;



//----------------------------------------------------------------------------------------------------------------------
// Group of sprite animations, meant to be used together based on the character's state or action (e.g. idle, walk, run, attack, etc.)
// For instance, the idle animation may be a group of 4 anims, one for each direction (NSEW)
//
class SpriteAnimationGroup
{
	friend class GridSpriteSheet;

public:

	SpriteAnimationGroup() = default;

	void LoadFromXml(void const* xmlElement);

	Name GetName() const;
	std::vector<SpriteAnimationDef> const& GetAnimationDefs() const;

	// tiebreakerDir is used as a tiebreaker if two animations have the same dot product with 'direction'
	SpriteAnimationDef const* GetBestAnimForDir(Vec2 const& direction, Vec2 const& tiebreakerDir) const;

protected:

	Name m_name;
	std::vector<SpriteAnimationDef> m_animationDefs;
};