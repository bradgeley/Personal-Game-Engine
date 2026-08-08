// Bradley Christensen - 2022-2026
#pragma once
#include "GameCommon.h"



//----------------------------------------------------------------------------------------------------------------------
class SCRunData
{
public:

	float m_gold					= StaticGameSettings::s_baseGold;
	float m_savingsInterestRate		= StaticGameSettings::s_baseSavingsInterestRate;
	float m_debtInterestRate		= StaticGameSettings::s_baseDebtInterestRate;
	float m_interestTimerSeconds	= StaticGameSettings::s_baseInterestTimerSeconds;
	float m_maxHealth				= StaticGameSettings::s_basePlayerHealth;
	float m_currentHealth			= StaticGameSettings::s_basePlayerHealth;
	float m_healthRegen				= StaticGameSettings::s_basePlayerHealthRegen;
};

