// Bradley Christensen - 2022-2026
#include "NoiseParams.h"



//----------------------------------------------------------------------------------------------------------------------
NoiseParams::NoiseParams(XmlElement const* xmlElement)
{
	m_outputRange = XmlUtils::ParseXmlAttribute(*xmlElement, "outputRange", m_outputRange);
	m_scale = XmlUtils::ParseXmlAttribute(*xmlElement, "scale", m_scale);
	m_numOctaves = XmlUtils::ParseXmlAttribute(*xmlElement, "numOctaves", m_numOctaves);
	m_octavePersistence = XmlUtils::ParseXmlAttribute(*xmlElement, "octavePersistence", m_octavePersistence);
	m_octaveScale = XmlUtils::ParseXmlAttribute(*xmlElement, "octaveScale", m_octaveScale);
	m_renormalize = XmlUtils::ParseXmlAttribute(*xmlElement, "renormalize", m_renormalize);
	m_seedOffset = XmlUtils::ParseXmlAttribute(*xmlElement, "seedOffset", m_seedOffset);
}