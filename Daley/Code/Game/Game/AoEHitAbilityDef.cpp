// Bradley Christensen - 2022-2026
#include "AoEHitAbilityDef.h"
#include "Ability.h"
#include "AoEHitAbility.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AoEHitAbilityDef::AoEHitAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    if (XmlElement const* cooldownElem = elem.FirstChildElement("Cooldown"))
    {
        m_cooldownDef = AbilityCooldownComponentDef(cooldownElem);
    }
    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef = AbilityTargetingComponentDef(targetingElem);
    }
    if (XmlElement const* critElem = elem.FirstChildElement("Crit"))
    {
        m_critDef = AbilityCritComponentDef(critElem);
    }
    if (XmlElement const* aoeHitElem = elem.FirstChildElement("Splash"))
    {
        m_splashDef = AbilitySplashComponentDef(aoeHitElem);
    }
    if (XmlElement const* puddleElem = elem.FirstChildElement("Puddle"))
    {
        m_puddleDef = AbilityPuddleComponentDef(puddleElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AoEHitAbilityDef::MakeAbilityInstance() const
{
    AoEHitAbility* ability = new AoEHitAbility(*this);
    return ability;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef* AoEHitAbilityDef::Copy() const
{
    return new AoEHitAbilityDef(*this);
}