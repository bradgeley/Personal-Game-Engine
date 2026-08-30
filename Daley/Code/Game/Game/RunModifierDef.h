// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Core/XmlUtils.h"
#include <array>



struct SystemContext;
struct RunModifier;



//----------------------------------------------------------------------------------------------------------------------
struct RunModifierDef
{
public:

	RunModifierDef(XmlElement const& modElement);
	virtual ~RunModifierDef() = default;
	static RunModifierDef const* MakeFromXml(XmlElement const& modElement);
	virtual RunModifier* MakeModifierInstance() const = 0;

public:

	static constexpr int s_maxRequirements = 3;

	Name m_name = Name::Invalid;
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

	TowerUnlockRunModifierDef const& GetDef() const;
};