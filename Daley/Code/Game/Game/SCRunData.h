// Bradley Christensen - 2022-2026
#pragma once
#include "GameCommon.h"
#include "RunModifierPool.h"
#include "Engine/Core/Name.h"
#include <array>



struct TowerPlacementRequest;
struct TowerSwirlRequest;



constexpr int MAX_MODIFIER_CHOICES = 3;



//----------------------------------------------------------------------------------------------------------------------
enum class GameMode
{
	Endless,
	Journey
};



//----------------------------------------------------------------------------------------------------------------------
struct PlaceableTower
{
	Name  m_towerName = Name::Invalid;
	Name  m_flavorName = Name::Invalid;
	char  m_hotkey = '0';
	float m_baseCost = 0.f;
	float m_cost = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct MissionGenData
{
	Name m_biomeName = Name::Invalid;
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
	void ResetModifiableAttributes();
	void PrepareForMissionStart();
	void OnTowerPlacementSuccess(TowerPlacementRequest const& request);
	void OnSwirlSuccess(TowerSwirlRequest const& request);

public:

	// Static Run Data
	RunModifierPool m_runModifierPool;

	// Generated Run Data
	uint32_t	m_seed = 0; // Mission seed = seed + missionIndex
	GameMode    m_mode = GameMode::Journey;
	std::array<MissionGenData, StaticGameSettings::s_numMissionsForVictory> m_missionGenData;

	// Non-Resettable Data (carries over from mission to mission)
	int			m_missionIndex					= 0;
	uint64_t 	m_experience					= 0;
	float		m_fractionalExperience			= 0.f; // overflow exp from multiplier increases
	int			m_numModifierChoicesRemaining	= 0;
	int			m_numModifierChoicesCompleted	= 0;
	std::vector<RunModifier*> m_activeRunModifiers;
	std::array<PlaceableTower, StaticGameSettings::s_flavorUnlockLimit + 1> m_placeableTowers = { "Wall1x1", Name::Invalid, 'T', 5.f, 5.f }; // +1 is for walls, bc they are not a flavor. Start with wall unlocked always.
	std::array<uint8_t, StaticGameSettings::s_flavorUnlockLimit + 1> m_towerPlacementKeyBindings = { 'T', 'Q', 'E', 'R', 'Y' }; // todo: move to some kind of user settings, out of run data
	
	// Resettable Data (resets at the start of each mission)
	float		m_gold							= StaticGameSettings::s_baseGold;
	float		m_health						= StaticGameSettings::s_basePlayerHealth;
	float		m_interestTimerSecondsRemaining	= StaticGameSettings::s_baseInterestTimerSeconds;
	int			m_numSoldTowers					= 0;
	int			m_numActiveModifierChoices		= 0;
	std::array<RunModifierDef const*, MAX_MODIFIER_CHOICES> m_modifierChoices;

	// Modifiable Data (carries over from mission to mission)
	float 		m_goldGainMultiplier	= 1.f;
	float		m_experienceMultiplier	= 1.f;
	float		m_income				= StaticGameSettings::s_baseIncome;
	float		m_creditLimit			= StaticGameSettings::s_baseCreditLimit;
	float		m_savingsInterestRate	= StaticGameSettings::s_baseSavingsInterestRate;
	float		m_debtInterestRate		= StaticGameSettings::s_baseDebtInterestRate;
	float		m_interestTimerSeconds	= StaticGameSettings::s_baseInterestTimerSeconds;
	float		m_maxHealth				= StaticGameSettings::s_basePlayerHealth;
	float		m_healthRegen			= StaticGameSettings::s_basePlayerHealthRegen;
	int			m_maxFlavors			= StaticGameSettings::s_flavorUnlockLimit;
	int			m_swirlLimit			= StaticGameSettings::s_baseSwirlLimit;
	float		m_costRampPerTower		= StaticGameSettings::s_baseCostRampPerTower;
	float		m_sellRefundRatio		= StaticGameSettings::s_baseSellRefundRatio;
	int			m_maxSellsPerMission	= StaticGameSettings::s_baseSellCount;

	// Persistent State (carries over from mission to mission)
	float m_currentTimeDilation = 1.f;

	// Transient State
	bool m_wantsModifierChoice = true;
	bool m_needsModifierRecalculation = true;
};



//----------------------------------------------------------------------------------------------------------------------
struct SCRunData
{
	RunData* m_data = nullptr; // Pointer to the current run, which is held in the RunState.
};