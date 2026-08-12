// Bradley Christensen - 2022-2026
#pragma once
#include "GameCommon.h"



//----------------------------------------------------------------------------------------------------------------------
struct SCRunData
{
public:

	// Data that carries over from mission to mission
	int	  m_seed					= 0; // Mission seed = seed + missionIndex
	int   m_missionIndex			= 0;
	int   m_totalMissions			= StaticGameSettings::s_numMissionsForVictory;
	float m_startingGold			= StaticGameSettings::s_baseGold;
	float m_creditLimit				= StaticGameSettings::s_baseCreditLimit;
	float m_savingsInterestRate		= StaticGameSettings::s_baseSavingsInterestRate;
	float m_debtInterestRate		= StaticGameSettings::s_baseDebtInterestRate;
	float m_maxHealth				= StaticGameSettings::s_basePlayerHealth;
	float m_interestTimerSeconds	= StaticGameSettings::s_baseInterestTimerSeconds;
	float m_healthRegen				= StaticGameSettings::s_basePlayerHealthRegen;

	// Data that resets each mission
	float m_gold							= StaticGameSettings::s_baseGold;
	float m_interestTimerSecondsRemaining	= StaticGameSettings::s_baseInterestTimerSeconds;
	float m_currentHealth					= StaticGameSettings::s_basePlayerHealth;
};