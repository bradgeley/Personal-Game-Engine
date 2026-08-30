// Bradley Christensen - 2022-2026
#pragma once
#include "GameCommon.h"
#include "RunModifierPool.h"
#include "Engine/Core/Name.h"
#include <array>



constexpr int MAX_PURCHASEABLE_TOWERS = 10;
constexpr int MAX_MODIFIER_CHOICES = 3;



//----------------------------------------------------------------------------------------------------------------------
struct PlaceableTower
{
	Name  m_towerName = Name::Invalid;
	char  m_hotkey = '0';
	float m_cost = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct MissionGenData
{
	Name m_mapName = Name::Invalid;
};



//----------------------------------------------------------------------------------------------------------------------
struct ExperienceLevelData
{
	bool m_isMaxLevel = false;
	int m_level = 0;
	float m_percentIntoLevel = 0.f;
	uint64_t m_experienceIntoLevel = 0;
	uint64_t m_experienceNeededForNextLevel = 0;
	uint64_t m_totalExperienceForNextLevel = 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct RunData
{
public:

	void Shutdown();

	static ExperienceLevelData GetLevelData(uint64_t experience);

	void GenerateModifierChoices();

	// Data that carries over from mission to mission
	uint32_t	m_seed					= 0; // Mission seed = seed + missionIndex
	int			m_missionIndex			= 0;
	uint64_t 	m_experience			= 0;
	float		m_startingGold			= StaticGameSettings::s_baseGold;
	float		m_creditLimit			= StaticGameSettings::s_baseCreditLimit;
	float		m_savingsInterestRate	= StaticGameSettings::s_baseSavingsInterestRate;
	float		m_debtInterestRate		= StaticGameSettings::s_baseDebtInterestRate;
	float		m_maxHealth				= StaticGameSettings::s_basePlayerHealth;
	float		m_interestTimerSeconds	= StaticGameSettings::s_baseInterestTimerSeconds;
	float		m_healthRegen			= StaticGameSettings::s_basePlayerHealthRegen;
	
	float		m_gold							= StaticGameSettings::s_baseGold;
	float		m_interestTimerSecondsRemaining	= StaticGameSettings::s_baseInterestTimerSeconds;
	float		m_health						= StaticGameSettings::s_basePlayerHealth;
	int			m_numSoldTowers					= 0;

	std::array<PlaceableTower, MAX_PURCHASEABLE_TOWERS> m_placeableTowers;
	std::array<MissionGenData, StaticGameSettings::s_numMissionsForVictory> m_missionGenData;

	std::vector<RunModifier*> m_activeRunModifiers;

	RunModifierPool m_runModifierPool;
	int m_numModifierChoicesRemaining = 0;
	int m_numModifierChoicesCompleted = 0;

	bool m_hasGeneratedModifierChoices = false;
	std::array<RunModifierDef const*, MAX_MODIFIER_CHOICES> m_modifierChoices;
};



//----------------------------------------------------------------------------------------------------------------------
struct SCRunData
{
	RunData* m_data = nullptr;
};