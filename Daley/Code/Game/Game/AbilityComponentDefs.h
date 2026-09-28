// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Renderer/Rgba8.h"



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCooldownComponentDef
{
	AbilityCooldownComponentDef() = default;
	explicit AbilityCooldownComponentDef(void const* xmlElement);

	float m_cooldownSeconds = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityTargetingComponentDef
{
	AbilityTargetingComponentDef() = default;
	explicit AbilityTargetingComponentDef(void const* xmlElement);

	float m_maxRange = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCritComponentDef
{
	AbilityCritComponentDef() = default;
	explicit AbilityCritComponentDef(void const* xmlElement);

	float m_critChance = 0.f;
	float m_critMulti = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityScalingComponentDef
{
	AbilityScalingComponentDef() = default;
	explicit AbilityScalingComponentDef(void const* xmlElement);

	float m_physical	= 0.f;
	float m_burn		= 0.f;
	float m_poison		= 0.f;
	float m_slow		= 0.f;
	float m_haste		= 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityChainComponentDef
{
	AbilityChainComponentDef() = default;
	explicit AbilityChainComponentDef(void const* xmlElement);

	int		m_maxChains = 0;
	float	m_chainChance = 1.f;
	float	m_chainDistance = 3.f;
	float	m_chainPayloadMulti = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityMultishotComponentDef
{
	AbilityMultishotComponentDef() = default;
	explicit AbilityMultishotComponentDef(void const* xmlElement);

	int	m_additionalTargets = 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityRenderComponentDef
{
	AbilityRenderComponentDef() = default;
	explicit AbilityRenderComponentDef(void const* xmlElement);

	Rgba8 m_tint = Rgba8::White;
	float m_depth = 0.f;
	float m_renderDuration = -1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEHitComponentDef
{
	AbilityAoEHitComponentDef() = default;
	explicit AbilityAoEHitComponentDef(void const* xmlElement);

	float m_radius = 0.f;
	AbilityScalingComponentDef	m_scaling;
	AbilityRenderComponentDef	m_renderDef;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEEffectComponentDef
{
	AbilityAoEEffectComponentDef() = default;
	explicit AbilityAoEEffectComponentDef(void const* xmlElement);

	Name m_aoeEffectDefName	= Name::Invalid;
	float m_radius = 0.f;
	float m_durationSeconds = -1.f; // negative == infinite
	AbilityScalingComponentDef	m_scaling;
	AbilityRenderComponentDef	m_renderDef;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityOnHitComponentDef
{
	AbilityOnHitComponentDef() = default;
	explicit AbilityOnHitComponentDef(void const* xmlElement);

	AbilityScalingComponentDef		m_scaling;
	AbilityAoEHitComponentDef		m_aoeHitOnHit;
	AbilityAoEEffectComponentDef	m_aoeEffectOnHit;
};