// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Core/XmlUtils.h"
#include <array>
#include <string>



class Ability;
struct CTags;
struct RunModifier;
struct SystemContext;



//----------------------------------------------------------------------------------------------------------------------
struct RunModifierDef
{
public:

	RunModifierDef(XmlElement const& modElement);
	virtual ~RunModifierDef() = default;
	static RunModifierDef const* MakeFromXml(XmlElement const& modElement);
	virtual RunModifier* MakeModifierInstance() const = 0;
	virtual void AppendDebugString(std::string& outStr) const;
	virtual void GetDescription(std::string& outStr) const = 0;

public:

	static constexpr int s_maxRequirements = 3;

	Name m_name = Name::Invalid;
	Name m_displayName = Name::Invalid;
	int m_levelRequirement = 0;
	int m_maxLevel = 1;
	float m_weight = 1.f;
	std::array<Name, s_maxRequirements> m_requirements;
};



//----------------------------------------------------------------------------------------------------------------------
struct RunModifier
{
public:

	RunModifier(RunModifierDef const& def);
	virtual ~RunModifier() = default;

	virtual void Apply(SystemContext const& context) const = 0;
	virtual void ApplyToAbility(Ability& ability, CTags const& tags) const;

	virtual void AppendDebugString(std::string& outStr) const;

public:

	RunModifierDef const& m_def;
	int m_level = 1;
};



//----------------------------------------------------------------------------------------------------------------------
struct TowerUnlockRunModifierDef : public RunModifierDef
{
public:

	TowerUnlockRunModifierDef(XmlElement const& modElement);

	virtual RunModifier* MakeModifierInstance() const override;

	virtual void AppendDebugString(std::string& outStr) const override;
	virtual void GetDescription(std::string& outStr) const override;

public:

	// Allows placement of this tower
	Name m_towerName = Name::Invalid;
	char m_defaultKey = '1';
	float m_baseCost = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct TowerUnlockRunModifier : public RunModifier
{
public:

	TowerUnlockRunModifier(TowerUnlockRunModifierDef const& def);

	virtual void Apply(SystemContext const& context) const override;

	virtual void AppendDebugString(std::string& outStr) const override;

	TowerUnlockRunModifierDef const& GetDef() const;
};



//----------------------------------------------------------------------------------------------------------------------
struct TowerPayloadRunModifierDef : public RunModifierDef
{
public:

	TowerPayloadRunModifierDef(XmlElement const& modElement);

	virtual RunModifier* MakeModifierInstance() const override;

	virtual void AppendDebugString(std::string& outStr) const override;
	virtual void GetDescription(std::string& outStr) const override;

public:

	// Linear increase per level of one payload type
	uint8_t m_payloadDamageTypeFlags = 0;
	float m_multiplierIncreaseBase = 1.f;
	float m_multiplierIncreasePerLevel = 1.f;
	std::array<Name, s_maxRequirements> m_tagRequirements; // this modifier applies to towers with these tags
};



//----------------------------------------------------------------------------------------------------------------------
struct TowerPayloadRunModifier : public RunModifier
{
public:

	TowerPayloadRunModifier(TowerPayloadRunModifierDef const& def);

	virtual void Apply(SystemContext const& context) const override;
	virtual void ApplyToAbility(Ability& ability, CTags const& tags) const override;

	virtual void AppendDebugString(std::string& outStr) const override;

	TowerPayloadRunModifierDef const& GetDef() const;
};