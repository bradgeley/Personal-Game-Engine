// Bradley Christensen - 2022-2026
#include "LaserAbilityDef.h"
#include "Ability.h"
#include "LaserAbility.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
LaserAbilityDef::LaserAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef = AbilityTargetingComponentDef(targetingElem);
    }
    if (XmlElement const* onHitElem = elem.FirstChildElement("OnHit"))
    {
        m_onHitDef = AbilityOnHitComponentDef(onHitElem);
	}
    if (XmlElement const* renderElem = elem.FirstChildElement("Render"))
    {
        m_renderDef = AbilityRenderComponentDef(renderElem);
	}
    if (XmlElement const* chainElem = elem.FirstChildElement("Chain"))
    {
        m_chainDef = AbilityChainComponentDef(chainElem);
    }
    if (XmlElement const* multishotElem = elem.FirstChildElement("Multishot"))
    {
        m_multishotDef = AbilityMultishotComponentDef(multishotElem);
    }
}



//----------------------------------------------------------------------------------------------------------------------
Ability* LaserAbilityDef::MakeAbilityInstance() const
{
    LaserAbility* ability = new LaserAbility(*this);
	return ability;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef* LaserAbilityDef::Copy() const
{
    return new LaserAbilityDef(*this);
}