// Bradley Christensen - 2022-2026
#pragma once
#include "AttributeSet.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/ECS/EntityID.h"
#include <vector>



struct SystemContext;



//----------------------------------------------------------------------------------------------------------------------
struct EnemyAffixDef
{
public:

	explicit EnemyAffixDef(XmlElement const& affixElement);

	bool ApplyToEntity(EntityID id, SystemContext const& context) const;

	static void LoadFromXML();
	static void Shutdown();
	static EnemyAffixDef const* GetAffixDef(Name name);
	static EnemyAffixDef const* GetRandom(int seed, bool rare = false, Name ignoreAffix = Name::Invalid);

private:

	static std::vector<EnemyAffixDef*> s_affixDefs;

public:

	Name m_name = Name::Invalid;

	// Grants an ability by name (optional, Name::Invalid if this affix does not grant an ability)
	Name m_grantedAbilityName = Name::Invalid;

	int m_weight = 1;
	bool m_rare = false;

	uint8_t m_healthFlags = 0;

	// Bonuses applied to the enemy that has this affix
	AttributeSet m_attributes;
};
