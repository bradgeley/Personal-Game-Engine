// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <cstdint>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
class Ability;



//----------------------------------------------------------------------------------------------------------------------
struct AdjacentHitAbilityDef;
struct AoEHitAbilityDef;
struct LaserAbilityDef;
struct PassiveAoEAbilityDef;
struct ProjectileHitAbilityDef;



//----------------------------------------------------------------------------------------------------------------------
struct AbilityDef
{
public:

	virtual ~AbilityDef() = default;
	AbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const = 0;
	virtual AbilityDef* Copy() const = 0;
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) = 0;

	virtual void SwirlInto(AbilityDef& other) const = 0;
	virtual bool ReceiveSwirl(ProjectileHitAbilityDef const& other) = 0;
	virtual bool ReceiveSwirl(AoEHitAbilityDef const& other) = 0;
	virtual bool ReceiveSwirl(PassiveAoEAbilityDef const& other) = 0;
	virtual bool ReceiveSwirl(LaserAbilityDef const& other) = 0;
	virtual bool ReceiveSwirl(AdjacentHitAbilityDef const& other) = 0;

public:

	static void LoadFromXML(Name filepath);
	static void SaveToXML(Name filepath);
	static void Shutdown();
	static AbilityDef const* GetAbilityDef(uint8_t id);
	static AbilityDef const* GetAbilityDef(Name name);
	static int GetAbilityDefID(Name name);
	static std::vector<AbilityDef*> const& GetAllAbilityDefs();

private:

	static std::vector<AbilityDef*> s_abilityDefs;

public:

	Name m_name = Name::Invalid;
};