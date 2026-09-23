// Bradley Christensen - 2022-2026
#include "AoEHitAbilityDef.h"
#include "Ability.h"
#include "AdjacentHitAbilityDef.h"
#include "LaserAbilityDef.h"
#include "PassiveAoEAbilityDef.h"
#include "ProjectileHitAbilityDef.h"
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
    if (XmlElement const* aoeHitElem = elem.FirstChildElement("AoEHit"))
    {
        m_aoeHitDef = AbilityAoEHitComponentDef(aoeHitElem);
    }
    if (XmlElement const* aoeEffectElem = elem.FirstChildElement("AoEEffect"))
    {
        m_aoeEffectDef = AbilityAoEEffectComponentDef(aoeEffectElem);
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



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbilityDef::WriteToXmlDoc(void* xmlDoc, void* rootElement)
{
    ASSERT_OR_DIE(xmlDoc != nullptr, "AoEHitAbilityDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(rootElement != nullptr, "AoEHitAbilityDef::WriteToXmlDoc - rootElement is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& rootElem = *static_cast<XmlElement*>(rootElement);

	// Ability
	XmlElement* parentElem = doc.NewElement("AoEHitAbilityDef");
	parentElem->SetAttribute("name", m_name.ToCStr());
	rootElem.InsertEndChild(parentElem);

	m_cooldownDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_targetingDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_critDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_aoeHitDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_aoeEffectDef.WriteToXmlDoc(xmlDoc, parentElem);
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbilityDef::SwirlInto(AbilityDef& other) const
{
	other.ReceiveSwirl(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void BlendDamageRanges(float& inOutMinDamageA, float& inOutMaxDamageA, float minDamageB, float maxDamageB, float cooldownA, float cooldownB)
{
	float dpsA = (inOutMinDamageA + inOutMaxDamageA) * 0.5f / cooldownA;
	float dpsB = (minDamageB + maxDamageB) * 0.5f / cooldownB;
	float averageCooldown = (cooldownA + cooldownB) * 0.5f;
	float averageDPS = (dpsA + dpsB) * 0.5f;
	float averageDamage = averageDPS * averageCooldown;
	float blendedMinDamage = (inOutMinDamageA + minDamageB) * 0.5f;
	float blendedMaxDamage = (inOutMaxDamageA + maxDamageB) * 0.5f;
	float blendedRangeAverage = (blendedMinDamage + blendedMaxDamage) * 0.5f;
	float damageScale = averageDamage / blendedRangeAverage;
	blendedMinDamage *= damageScale;
	blendedMaxDamage *= damageScale;
	inOutMinDamageA = blendedMinDamage;
	inOutMaxDamageA = blendedMaxDamage;
}



//----------------------------------------------------------------------------------------------------------------------
void BlendPayloadValue(float& inOutDamageA, float damageB, float cooldownA, float cooldownB)
{
	float dpsA = inOutDamageA / cooldownA;
	float dpsB = damageB / cooldownB;
	float averageDPS = (dpsA + dpsB) * 0.5f;
	float averageCooldown = (cooldownA + cooldownB) * 0.5f;
	float blendedDamage = averageDPS * averageCooldown;
	inOutDamageA = blendedDamage;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(ProjectileHitAbilityDef const& other)
{
	float const cooldownA = m_cooldownDef.m_cooldownSeconds;
	float const cooldownB = other.m_cooldownDef.m_cooldownSeconds;
	float const cooldownRatio = cooldownA / cooldownB;

	float const projHitAverageSplashDamage = other.m_onHitDef.m_aoeHitOnHit.m_damageOnHit.GetAverageDamage();
	float const projAoEEffectDamagePerSecond = other.m_onHitDef.m_aoeEffectOnHit.m_damagePerSecond.m_maxDamage;

    // Cooldown (Average)
    m_cooldownDef.m_cooldownSeconds = (cooldownA + cooldownB) * 0.5f;

    // Targeting (Fixed Loss Ratio)
	m_targetingDef.m_minRange *= 0.75f;
	m_targetingDef.m_maxRange *= 0.75f;

    // Crit (Average)
	m_critDef.m_critChance = (m_critDef.m_critChance + other.m_critDef.m_critChance) * 0.5f;
	m_critDef.m_critMulti = (m_critDef.m_critMulti + other.m_critDef.m_critMulti) * 0.5f;

    // Proj Hit -> My AoE Hit
	BlendDamageRanges(m_aoeHitDef.m_damageOnHit.m_minDamage, m_aoeHitDef.m_damageOnHit.m_maxDamage, other.m_onHitDef.m_damageOnHit.m_minDamage,
		other.m_onHitDef.m_damageOnHit.m_maxDamage, cooldownA, cooldownB);
	BlendPayloadValue(m_aoeHitDef.m_poisonOnHit.m_poison, other.m_onHitDef.m_poisonOnHit.m_poison, cooldownA, cooldownB);
	BlendPayloadValue(m_aoeHitDef.m_burnOnHit.m_burn, other.m_onHitDef.m_burnOnHit.m_burn, cooldownA, cooldownB);
	BlendPayloadValue(m_aoeHitDef.m_slowOnHit.m_duration, other.m_onHitDef.m_slowOnHit.m_duration, cooldownA, cooldownB);

	// Proj AoE Hit -> My AoE Hit
	if (other.m_onHitDef.m_aoeHitOnHit.m_radius > 0.f)
	{
		m_aoeHitDef.m_damageOnHit.m_minDamage += projHitAverageSplashDamage * cooldownRatio;
		m_aoeHitDef.m_damageOnHit.m_maxDamage += projHitAverageSplashDamage * cooldownRatio;
		m_aoeHitDef.m_poisonOnHit.m_poison += other.m_onHitDef.m_aoeHitOnHit.m_poisonOnHit.m_poison * cooldownRatio;
		m_aoeHitDef.m_burnOnHit.m_burn += other.m_onHitDef.m_aoeHitOnHit.m_burnOnHit.m_burn * cooldownRatio;
		m_aoeHitDef.m_slowOnHit.m_duration += other.m_onHitDef.m_aoeHitOnHit.m_slowOnHit.m_duration * cooldownRatio;
	}

	// Proj AoE Effect -> My AoE Effect
	if (other.m_onHitDef.m_aoeEffectOnHit.m_aoeEffectDefName != Name::Invalid)
	{
		m_aoeEffectDef.m_aoeEffectDefName = other.m_onHitDef.m_aoeEffectOnHit.m_aoeEffectDefName;
	}

	m_aoeEffectDef.m_radius = (m_aoeEffectDef.m_radius + other.m_onHitDef.m_aoeEffectOnHit.m_radius) * 0.5f;
	m_aoeEffectDef.m_durationSeconds = (m_aoeEffectDef.m_durationSeconds + other.m_onHitDef.m_aoeEffectOnHit.m_durationSeconds) * 0.5f;
	BlendDamageRanges(m_aoeEffectDef.m_damagePerSecond.m_minDamage, m_aoeEffectDef.m_damagePerSecond.m_maxDamage, projAoEEffectDamagePerSecond,
		projAoEEffectDamagePerSecond, cooldownA, cooldownB);
	BlendPayloadValue(m_aoeEffectDef.m_poisonPerSecond.m_poison, other.m_onHitDef.m_aoeEffectOnHit.m_poisonPerSecond.m_poison, cooldownA, cooldownB);
	BlendPayloadValue(m_aoeEffectDef.m_burnPerSecond.m_burn, other.m_onHitDef.m_aoeEffectOnHit.m_burnPerSecond.m_burn, cooldownA, cooldownB);
	BlendPayloadValue(m_aoeEffectDef.m_slowPerSecond.m_duration, other.m_onHitDef.m_aoeEffectOnHit.m_slowPerSecond.m_duration, cooldownA, cooldownB);

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(AoEHitAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(PassiveAoEAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(LaserAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(AdjacentHitAbilityDef const& other)
{
    return false;
}
