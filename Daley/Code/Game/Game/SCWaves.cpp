// Bradley Christensen - 2022-2026
#include "SCWaves.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
float LevelWaveGenDef::GetHealthScaling(int waveIndex) const
{
	float waveIndexForScaling = static_cast<float>(waveIndex);
	float multiplyAdditiveHealthScaling = (waveIndexForScaling * m_waveGenModifiers.m_healthMultiplierIncreasePerWave);
	float exponentialHealthScaling = MathUtils::PowF(m_waveGenModifiers.m_healthExponentialScalingPerWave, waveIndexForScaling + 1.f);
	float healthScaling = (1.f + multiplyAdditiveHealthScaling) * exponentialHealthScaling;
    return healthScaling;
}



//----------------------------------------------------------------------------------------------------------------------
float LevelWaveGenDef::GetSpeedScaling(int waveIndex) const
{
	float waveIndexForScaling = static_cast<float>(waveIndex);
	float multiplyAdditiveSpeedScaling = 1.f + (waveIndexForScaling * m_waveGenModifiers.m_speedMultiplierIncreasePerWave);
    return multiplyAdditiveSpeedScaling;
}



//----------------------------------------------------------------------------------------------------------------------
float LevelWaveGenDef::GetMagicEnemyChance(int waveIndex) const
{
	float waveIndexForScaling = static_cast<float>(waveIndex);
	float multiplyAdditiveMagicEnemyChance = (waveIndexForScaling * m_waveGenModifiers.m_magicEnemyChanceIncreasePerWave);
	return m_waveGenModifiers.m_magicEnemyChance * (1.f + multiplyAdditiveMagicEnemyChance);
}



//----------------------------------------------------------------------------------------------------------------------
float LevelWaveGenDef::GetRareEnemyChance(int waveIndex) const
{
	float waveIndexForScaling = static_cast<float>(waveIndex);
	float multiplyAdditiveRareEnemyChance = (waveIndexForScaling * m_waveGenModifiers.m_rareEnemyChanceIncreasePerWave);
	return m_waveGenModifiers.m_rareEnemyChance * (1.f + multiplyAdditiveRareEnemyChance);
}