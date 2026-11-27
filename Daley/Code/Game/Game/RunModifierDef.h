// Bradley Christensen - 2022-2026
#pragma once
#include "AttributeSet.h"
#include "GameCommon.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/XmlUtils.h"
#include <array>
#include <string>



struct CAbility;
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
	virtual bool AreRequirementsMet(RunData const& runData) const;

public:

	static constexpr int s_maxRequirements = 3;

	Name m_name = Name::Invalid;
	int m_levelRequirement = 0;
	int m_maxLevel = 1;
	float m_weight = 1.f;
	std::array<Name, s_maxRequirements> m_requirements;
	RunModifierDisplayData m_displayData;
	RunModifierDisplayData m_secondaryDisplayData;
};



//----------------------------------------------------------------------------------------------------------------------
struct RunModifier
{
public:

	RunModifier(RunModifierDef const& def);
	virtual ~RunModifier() = default;

	virtual void Apply(SystemContext const& context) const = 0;
	virtual void ApplyToAbility(CAbility& ability, CTags const& tags) const;
	virtual void ApplyToRunData(RunData& runData) const;

	virtual void GetDescription(std::string& outStr) const = 0;

public:

	RunModifierDef const& m_def;
	int m_level = 1;
};



//----------------------------------------------------------------------------------------------------------------------
struct FlavorUnlockRunModifierDef : public RunModifierDef
{
public:

	FlavorUnlockRunModifierDef(XmlElement const& modElement);

	virtual RunModifier* MakeModifierInstance() const override;

	virtual void GetDescription(std::string& outStr) const override;

	virtual bool AreRequirementsMet(RunData const& runData) const override;

public:

	// Allows placement of this tower
	Name m_flavorName = Name::Invalid;
	char m_forcedKey = 0;
	float m_baseCost = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct FlavorUnlockRunModifier : public RunModifier
{
public:

	FlavorUnlockRunModifier(FlavorUnlockRunModifierDef const& def);

	virtual void Apply(SystemContext const& context) const override;
	virtual void ApplyToRunData(RunData& runData) const override;

	virtual void GetDescription(std::string& outStr) const override;

	FlavorUnlockRunModifierDef const& GetDef() const;
};



//----------------------------------------------------------------------------------------------------------------------
struct FlavorAbilityRunModifierDef : public RunModifierDef
{
public:

	FlavorAbilityRunModifierDef(XmlElement const& modElement);

	virtual RunModifier* MakeModifierInstance() const override;

	virtual void GetDescription(std::string& outStr) const override;

public:

	// Linear increase per level of one attribute
	AttributeSet m_modifiers;
	AttributeSet m_modifiersPerLevel;
	std::array<Name, s_maxRequirements> m_tagRequirements; // this modifier applies to towers with these tags
};



//----------------------------------------------------------------------------------------------------------------------
struct FlavorAbilityRunModifier : public RunModifier
{
public:

	FlavorAbilityRunModifier(FlavorAbilityRunModifierDef const& def);

	AttributeSet GetValue() const;

	virtual void Apply(SystemContext const& context) const override;
	virtual void ApplyToAbility(CAbility& ability, CTags const& tags) const override;

	virtual void GetDescription(std::string& outStr) const override;

	FlavorAbilityRunModifierDef const& GetDef() const;
};



//----------------------------------------------------------------------------------------------------------------------
struct MetaRunModifierDef : public RunModifierDef
{
public:

	MetaRunModifierDef(XmlElement const& modElement);

	virtual RunModifier* MakeModifierInstance() const override;

	virtual void GetDescription(std::string& outStr) const override;

public:

	AttributeSet m_modifiers;
	AttributeSet m_modifiersPerLevel;
};



//----------------------------------------------------------------------------------------------------------------------
struct MetaRunModifier : public RunModifier
{
public:

	MetaRunModifier(MetaRunModifierDef const& def);

	AttributeSet GetValue() const;

	virtual void Apply(SystemContext const& context) const override;
	virtual void ApplyToRunData(RunData& runData) const override;

	virtual void GetDescription(std::string& outStr) const override;

	MetaRunModifierDef const& GetDef() const;
};