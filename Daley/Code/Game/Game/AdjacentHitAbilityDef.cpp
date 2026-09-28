// Bradley Christensen - 2022-2026
#include "AdjacentHitAbilityDef.h"
#include "Ability.h"
#include "AdjacentHitAbility.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AdjacentHitAbilityDef::AdjacentHitAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    if (XmlElement const* cooldownElem = elem.FirstChildElement("Cooldown"))
    {
        m_cooldownDef = AbilityCooldownComponentDef(cooldownElem);
    }

    // targeting component is implicit, no data in it anyway

	if (XmlElement const* scalingElem = elem.FirstChildElement("Scaling"))
	{
		m_scaling = AbilityScalingComponentDef(scalingElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AdjacentHitAbilityDef::MakeAbilityInstance() const
{
	return new AdjacentHitAbility(*this);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef* AdjacentHitAbilityDef::Copy() const
{
    return new AdjacentHitAbilityDef(*this);
}
