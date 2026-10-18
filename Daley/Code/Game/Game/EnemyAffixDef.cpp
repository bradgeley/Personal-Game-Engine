// Bradley Christensen - 2022-2026
#include "EnemyAffixDef.h"
#include "Ability.h"
#include "AbilityDef.h"
#include "CAbility.h"
#include "CHealth.h"
#include "CMovement.h"
#include "CTags.h"
#include "CTime.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/Noise.h"



//----------------------------------------------------------------------------------------------------------------------
static const char* s_affixDefsFilePath = "Data/Definitions/EnemyAffixDefs.xml";
std::vector<EnemyAffixDef*> EnemyAffixDef::s_affixDefs;



//----------------------------------------------------------------------------------------------------------------------
EnemyAffixDef::EnemyAffixDef(XmlElement const& affixElement)
{
	m_name = XmlUtils::ParseXmlAttribute(affixElement, "name", m_name);
	m_grantedAbilityName = XmlUtils::ParseXmlAttribute(affixElement, "ability", m_grantedAbilityName);

	if (XmlElement const* healthFlagsElement = affixElement.FirstChildElement("HealthFlags"))
	{
		m_healthFlags = 0;
		bool cannotBeProtected = XmlUtils::ParseXmlAttribute(*healthFlagsElement, "cannotBeProtected", false);
		if (cannotBeProtected)
		{
			m_healthFlags |= static_cast<uint8_t>(HealthFlags::CannotBeProtected);
		}
	}

	if (XmlElement const* attributeBonusesElement = affixElement.FirstChildElement("Attributes"))
	{
		m_enemyAttributeBonuses = AbilityAttributes(attributeBonusesElement);
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool EnemyAffixDef::ApplyToEntity(EntityID id, SystemContext const& context) const
{
	float healthMultiplier = m_enemyAttributeBonuses.GetValue(AbilityAttribute::Health_Multi);
	float moveSpeedMultiplier = m_enemyAttributeBonuses.GetValue(AbilityAttribute::MoveSpeed_Multi);
	float physResist = m_enemyAttributeBonuses.GetValue(AbilityAttribute::PhysicalResistance);
	float burnResist = m_enemyAttributeBonuses.GetValue(AbilityAttribute::BurnResistance);
	float poisonResist = m_enemyAttributeBonuses.GetValue(AbilityAttribute::PoisonResistance);
	float slowResist = m_enemyAttributeBonuses.GetValue(AbilityAttribute::SlowResistance);

	bool relevantToHealth = (m_healthFlags != 0 || healthMultiplier != 0.f || physResist != 0.f || burnResist != 0.f || poisonResist != 0.f);
	bool relevantToMovement = (moveSpeedMultiplier != 0.f);
	bool relevantToTime = (slowResist != 0.f);

	CTags* tags = context.GetComponentUnsafe<CTags>(id);
	if (tags)
	{
		tags->AddTag(m_name);
	}

	if (relevantToHealth)
	{
		CHealth* health = context.GetComponentUnsafe<CHealth>(id);
		if (health)
		{
			health->m_healthFlags |= m_healthFlags;
			health->m_maxHealth *= (1.f + healthMultiplier);
			health->m_currentHealth = health->m_maxHealth;
			health->m_physicalResistance += physResist;
			health->m_burnResistance += burnResist;
			health->m_poisonResistance += poisonResist;
		}
	}

	if (relevantToMovement)
	{
		CMovement* movement = context.GetComponentUnsafe<CMovement>(id);
		if (movement)
		{
			movement->m_movementSpeedMultiplier += moveSpeedMultiplier;
		}
	}

	if (relevantToTime)
	{
		CTime* time = context.GetComponentUnsafe<CTime>(id);
		if (time)
		{
			time->m_slowResistance += slowResist;
		}
	}
	
	if (m_grantedAbilityName != Name::Invalid)
	{
		CAbility* abilityComponent = context.GetComponentUnsafe<CAbility>(id);
		if (!abilityComponent)
		{
			abilityComponent = context.AddComponentUnsafe<CAbility>(id);
		}
		ASSERT_OR_DIE(abilityComponent, StringUtils::StringF("EnemyAffixDef::ApplyToEntity - Entity %u does not have a CAbility component and could not be added.", id));

		AbilityDef const* abilityDef = AbilityDef::GetAbilityDef(m_grantedAbilityName);
		ASSERT_OR_DIE(abilityDef, StringUtils::StringF("EnemyAffixDef::ApplyToEntity - affix \"%s\" grants ability \"%s\" but that ability does not exist.", m_name.ToCStr(), m_grantedAbilityName.ToCStr()));
		Ability* newAbility = abilityDef->MakeAbilityInstance();
		ASSERT_OR_DIE(newAbility, StringUtils::StringF("EnemyAffixDef::ApplyToEntity - affix \"%s\" grants ability \"%s\" but that ability failed to instantiate.", m_name.ToCStr(), m_grantedAbilityName.ToCStr()));
		newAbility->Initialize(context, id);
		abilityComponent->m_abilities.push_back(newAbility);
	}

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
void EnemyAffixDef::LoadFromXML()
{
	if (s_affixDefs.empty())
	{
		XmlDocument affixDefinitionsDoc;
		affixDefinitionsDoc.LoadFile(s_affixDefsFilePath);
		ASSERT_OR_DIE(affixDefinitionsDoc.ErrorID() == tinyxml2::XML_SUCCESS, StringUtils::StringF("Failed to load AffixDefs XML file \"%s\". Error: %s", s_affixDefsFilePath, affixDefinitionsDoc.ErrorName()));

		XmlElement const* affixElement = affixDefinitionsDoc.RootElement()->FirstChildElement();
		while (affixElement)
		{
			EnemyAffixDef* affixDef = new EnemyAffixDef(*affixElement);
			s_affixDefs.push_back(affixDef);
			affixElement = affixElement->NextSiblingElement();
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void EnemyAffixDef::Shutdown()
{
	for (EnemyAffixDef* affixDef : s_affixDefs)
	{
		delete affixDef;
	}
	s_affixDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
EnemyAffixDef const* EnemyAffixDef::GetAffixDef(Name name)
{
	for (EnemyAffixDef const* affixDef : s_affixDefs)
	{
		if (affixDef->m_name == name)
		{
			return affixDef;
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
EnemyAffixDef const* EnemyAffixDef::GetRandom(int seed)
{
	if (s_affixDefs.empty())
	{
		return nullptr;
	}

	int randomInt = Noise::GetRandomIntInRange(0, static_cast<int>(s_affixDefs.size()) - 1, 0, seed);

	return s_affixDefs[randomInt];
}
