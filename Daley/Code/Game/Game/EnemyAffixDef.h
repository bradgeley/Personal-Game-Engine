// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityAttributes.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/ECS/EntityID.h"
#include <array>
#include <vector>



struct SystemContext;



//----------------------------------------------------------------------------------------------------------------------
struct EnemyAffixDef
{
public:

	explicit EnemyAffixDef(XmlElement const& affixElement);

	bool ApplyToEnemy(EntityID id, SystemContext& context);

	static void LoadFromXML();
	static void Shutdown();
	static EnemyAffixDef const* GetAffixDef(Name name);

private:

	static std::vector<EnemyAffixDef*> s_affixDefs;

public:

	Name m_name = Name::Invalid;

	// Grants an ability by name (optional, Name::Invalid if this affix does not grant an ability)
	Name m_grantedAbilityName = Name::Invalid;

	// Bonuses applied to the enemy that has this affix
	AbilityAttributes m_enemyAttributeBonuses;
};
