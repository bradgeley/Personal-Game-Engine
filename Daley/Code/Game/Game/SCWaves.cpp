// Bradley Christensen - 2022-2026
#include "SCWaves.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void LevelWaveGenDef::LoadFromXml(XmlElement const* xmlElement)
{
	XmlElement const* randomWaveStreamElement = xmlElement->FirstChildElement("RandomWaveStream");
	while (randomWaveStreamElement)
	{
		RandomWaveStreamDef randomWaveStreamDef;
		randomWaveStreamDef.m_overTimeSeconds = XmlUtils::ParseXmlAttribute(*randomWaveStreamElement, "time", randomWaveStreamDef.m_overTimeSeconds);
		randomWaveStreamDef.m_minNumEntities = XmlUtils::ParseXmlAttribute(*randomWaveStreamElement, "min", randomWaveStreamDef.m_minNumEntities);
		randomWaveStreamDef.m_maxNumEntities = XmlUtils::ParseXmlAttribute(*randomWaveStreamElement, "max", randomWaveStreamDef.m_maxNumEntities);
		randomWaveStreamDef.m_weight = XmlUtils::ParseXmlAttribute(*randomWaveStreamElement, "weight", randomWaveStreamDef.m_weight);

		std::string tags = XmlUtils::ParseXmlAttribute(*randomWaveStreamElement, "tags", "");
		Strings tagStrings = StringUtils::SplitStringOnDelimiter(tags, ',');
		for (std::string const& tagString : tagStrings)
		{
			randomWaveStreamDef.m_enemyTags.push_back(tagString);
		}

		m_randomWaves.push_back(randomWaveStreamDef);
		randomWaveStreamElement = randomWaveStreamElement->NextSiblingElement("RandomWaveStream");
	}

	XmlElement const* fixedWaveStreamElement = xmlElement->FirstChildElement("FixedWaveStream");
	while (fixedWaveStreamElement)
	{
		FixedWaveStreamDef fixedWaveStreamDef;
		fixedWaveStreamDef.m_waveIndex = XmlUtils::ParseXmlAttribute(*fixedWaveStreamElement, "waveIndex", fixedWaveStreamDef.m_waveIndex);
		fixedWaveStreamDef.m_overTimeSeconds = XmlUtils::ParseXmlAttribute(*fixedWaveStreamElement, "time", fixedWaveStreamDef.m_overTimeSeconds);
		fixedWaveStreamDef.m_recurAfterNumWaves = XmlUtils::ParseXmlAttribute(*fixedWaveStreamElement, "recurAfter", fixedWaveStreamDef.m_recurAfterNumWaves);
		fixedWaveStreamDef.m_numEntities = XmlUtils::ParseXmlAttribute(*fixedWaveStreamElement, "num", fixedWaveStreamDef.m_numEntities);
		fixedWaveStreamDef.m_entityName = XmlUtils::ParseXmlAttribute(*fixedWaveStreamElement, "entity", Name::Invalid);
		fixedWaveStreamDef.m_isGuaranteedLastWave = XmlUtils::ParseXmlAttribute(*fixedWaveStreamElement, "guaranteedLastWave", fixedWaveStreamDef.m_isGuaranteedLastWave);
		m_fixedWaves.push_back(fixedWaveStreamDef);
		fixedWaveStreamElement = fixedWaveStreamElement->NextSiblingElement("FixedWaveStream");
	}
}



//----------------------------------------------------------------------------------------------------------------------
float LevelWaveGenDef::GetHealthScaling(int waveIndex) const
{
	float waveIndexForScaling = static_cast<float>(waveIndex);
	float multiplyAdditiveHealthScaling = (waveIndexForScaling * m_waveGenModifiers.m_healthMultiplierIncreasePerWave);
	float exponentialHealthScaling = MathUtils::PowF(m_waveGenModifiers.m_healthExponentialScalingPerWave, waveIndexForScaling + 1.f);
	float multaplicativeScaling = m_waveGenModifiers.m_healthMultiplier;
	float healthScaling = (1.f + multiplyAdditiveHealthScaling) * exponentialHealthScaling * multaplicativeScaling;
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