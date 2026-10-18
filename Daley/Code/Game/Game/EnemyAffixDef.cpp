// Bradley Christensen - 2022-2026
#include "EnemyAffixDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
static const char* s_affixDefsFilePath = "Data/Definitions/Affixes.xml";
std::vector<EnemyAffixDef*> EnemyAffixDef::s_affixDefs;



//----------------------------------------------------------------------------------------------------------------------
EnemyAffixDef::EnemyAffixDef(XmlElement const& affixElement)
{
	m_name = XmlUtils::ParseXmlAttribute(affixElement, "name", m_name);
	m_grantedAbilityName = XmlUtils::ParseXmlAttribute(affixElement, "grantsAbility", m_grantedAbilityName);

	XmlElement const* attributeBonusesElement = affixElement.FirstChildElement("Attributes");
	if (attributeBonusesElement)
	{
		m_enemyAttributeBonuses = AbilityAttributes(attributeBonusesElement);
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool EnemyAffixDef::ApplyToEnemy(EntityID id, SystemContext& context)
{

	return false;
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