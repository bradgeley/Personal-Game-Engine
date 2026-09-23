// Bradley Christensen - 2022-2026
#include "AbilityDef.h"
#include "AdjacentHitAbilityDef.h"
#include "AoEHitAbilityDef.h"
#include "LaserAbilityDef.h"
#include "PassiveAoEAbilityDef.h"
#include "ProjectileHitAbilityDef.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
std::vector<AbilityDef*> AbilityDef::s_abilityDefs;



//----------------------------------------------------------------------------------------------------------------------
void AbilityDef::LoadFromXML(Name filepath)
{
    XmlDocument doc;
    doc.LoadFile(filepath.ToCStr());
    auto root = doc.RootElement();
    if (!root)
    {
		DevConsoleUtils::LogError("AbilityDef::LoadFromXml - Could not load file: %s", filepath.ToCStr());
        return;
    }

    // Projectile abilities
    XmlElement* abilityDefElem = root->FirstChildElement("ProjectileHitAbilityDef");
    while (abilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*abilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
			DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

		ProjectileHitAbilityDef* newDef = new ProjectileHitAbilityDef(abilityDefElem);
        s_abilityDefs.push_back(newDef);

        abilityDefElem = abilityDefElem->NextSiblingElement("ProjectileHitAbilityDef");
    }

    // AoE Hit Abilities
	XmlElement* aoeHitAbilityDefElem = root->FirstChildElement("AoEHitAbilityDef");
    while (aoeHitAbilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*aoeHitAbilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
            DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

        AoEHitAbilityDef* newDef = new AoEHitAbilityDef(aoeHitAbilityDefElem);
        s_abilityDefs.push_back(newDef);

        aoeHitAbilityDefElem = aoeHitAbilityDefElem->NextSiblingElement("AoEHitAbilityDef");
    }


	// Passive AoE Abilities
	XmlElement* passiveAoEAbilityDefElem = root->FirstChildElement("PassiveAoEAbilityDef");
    while (passiveAoEAbilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*passiveAoEAbilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
            DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

        PassiveAoEAbilityDef* newDef = new PassiveAoEAbilityDef(passiveAoEAbilityDefElem);
        s_abilityDefs.push_back(newDef);
        passiveAoEAbilityDefElem = passiveAoEAbilityDefElem->NextSiblingElement("PassiveAoEAbilityDef");
	}

    // Adjacent Hit Abilities
	XmlElement* adjacentHitAbilityDefElem = root->FirstChildElement("AdjacentHitAbilityDef");
    while (adjacentHitAbilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*adjacentHitAbilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
            DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

        AdjacentHitAbilityDef* newDef = new AdjacentHitAbilityDef(adjacentHitAbilityDefElem);
        s_abilityDefs.push_back(newDef);
        adjacentHitAbilityDefElem = adjacentHitAbilityDefElem->NextSiblingElement("AdjacentHitAbilityDef");
	}

    // Laser Abilities
	XmlElement* laserAbilityDefElem = root->FirstChildElement("LaserAbilityDef");
    while (laserAbilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*laserAbilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
            DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

        LaserAbilityDef* newDef = new LaserAbilityDef(laserAbilityDefElem);
        s_abilityDefs.push_back(newDef);
        laserAbilityDefElem = laserAbilityDefElem->NextSiblingElement("LaserAbilityDef");
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityDef::SaveToXML(Name filepath)
{
    XmlDocument doc;

	XmlElement* rootElem = doc.NewElement("AbilityDefs");
	doc.InsertFirstChild(rootElem);

	for (AbilityDef* def : s_abilityDefs)
	{
		def->WriteToXmlDoc(&doc, rootElem);
	}

	doc.SaveFile(filepath.ToCStr());
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityDef::Shutdown()
{
    for (AbilityDef* def : s_abilityDefs)
    {
        delete def;
    }
    s_abilityDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef const* AbilityDef::GetAbilityDef(uint8_t id)
{
    size_t index = static_cast<size_t>(id);
    return s_abilityDefs[index];
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef const* AbilityDef::GetAbilityDef(Name name)
{
    for (size_t i = 0; i < s_abilityDefs.size(); i++)
    {
        AbilityDef const* def = s_abilityDefs[i];
        if (def->m_name == name)
        {
            return def;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
int AbilityDef::GetAbilityDefID(Name name)
{
    for (size_t i = 0; i < s_abilityDefs.size(); i++)
    {
        AbilityDef const* def = s_abilityDefs[i];
        if (def->m_name == name)
        {
            return (int) i;
        }
    }
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<AbilityDef*> const& AbilityDef::GetAllAbilityDefs()
{
	return s_abilityDefs;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef::AbilityDef(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_name = XmlUtils::ParseXmlAttribute(elem, "name", m_name);
}