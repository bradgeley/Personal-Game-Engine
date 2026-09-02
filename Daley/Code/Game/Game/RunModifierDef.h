// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Core/XmlUtils.h"
#include <array>
#include <string>



class Ability;
struct CTags;
struct RunData;
struct RunModifier;
struct SCRunData;
struct SystemContext;



//----------------------------------------------------------------------------------------------------------------------
struct RunModifierDisplayData
{
	RunModifierDisplayData() = default;
	RunModifierDisplayData(XmlElement const& element);

	Name m_displayName = "Invalid RunModifier";
	Name m_spriteSheet = Name::Invalid;
	Name m_anim = Name::Invalid;
	Rgba8 m_tint = Rgba8::White;
};



//----------------------------------------------------------------------------------------------------------------------
struct RunModifierDef
{
public:

	RunModifierDef(XmlElement const& modElement);
	virtual ~RunModifierDef() = default;
	static RunModifierDef const* MakeFromXml(XmlElement const& modElement);
	virtual RunModifier* MakeModifierInstance() const = 0;
	virtual void GetDescription(std::string& outStr) const = 0;

public:

	static constexpr int s_maxRequirements = 3;

	Name m_name = Name::Invalid;
	Name m_description = Name::Invalid;
	int m_levelRequirement = 0;
	int m_maxLevel = 1;
	float m_weight = 1.f;
	std::array<Name, s_maxRequirements> m_requirements;
	RunModifierDisplayData m_displayData;
};



//----------------------------------------------------------------------------------------------------------------------
struct RunModifier
{
public:

	RunModifier(RunModifierDef const& def);
	virtual ~RunModifier() = default;

	virtual void Apply(SystemContext const& context) const = 0;
	virtual void ApplyToAbility(Ability& ability, CTags const& tags) const;
	virtual void ApplyToRunData(RunData& runData) const;

	virtual void GetDescription(std::string& outStr) const = 0;

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

	virtual void GetDescription(std::string& outStr) const override;

	TowerUnlockRunModifierDef const& GetDef() const;
};



//----------------------------------------------------------------------------------------------------------------------
struct TowerPayloadRunModifierDef : public RunModifierDef
{
public:

	TowerPayloadRunModifierDef(XmlElement const& modElement);

	virtual RunModifier* MakeModifierInstance() const override;

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

	virtual void GetDescription(std::string& outStr) const override;

	TowerPayloadRunModifierDef const& GetDef() const;
};



//----------------------------------------------------------------------------------------------------------------------
struct EconomyRunModifierDef : public RunModifierDef
{
public:

	EconomyRunModifierDef(XmlElement const& modElement);

	virtual RunModifier* MakeModifierInstance() const override;

	virtual void GetDescription(std::string& outStr) const override;

public:

	// Gold gain
	float m_goldGainMultiplierIncreaseBase = 0.f;
	float m_goldGainMultiplierIncreasePerLevel = 0.f;

	// Interest
	float m_interestRateIncreaseBase = 0.f;
	float m_interestRateIncreasePerLevel = 0.f;
	float m_interestTimerSpeedIncreaseBase = 0.f;
	float m_interestTimerSpeedIncreasePerLevel = 0.f;

	// Selling
	int m_baseSells = 0;
	int m_sellsPerLevel = 0;
	float m_baseSellRateIncrease = 0.f;
	float m_sellRateIncreasePerLevel = 0.f;

	// Debt
	float m_creditLimitIncreaseBase = 0.f;
	float m_creditLimitIncreasePerLevel = 0.f;
	float m_debtInterestRateDecreaseBase = 0.f;
	float m_debtInterestRateDecreasePerLevel = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct EconomyRunModifier : public RunModifier
{
public:

	EconomyRunModifier(EconomyRunModifierDef const& def);

	virtual void Apply(SystemContext const& context) const override;
	virtual void ApplyToRunData(RunData& runData) const override;

	virtual void GetDescription(std::string& outStr) const override;

	EconomyRunModifierDef const& GetDef() const;
};