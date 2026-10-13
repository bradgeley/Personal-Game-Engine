// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/XmlUtils.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct NoiseParams
{
	NoiseParams() = default;
	NoiseParams(XmlElement const* xmlElement);

	Vec2	 m_outputRange = Vec2(0.f, 1.f); // Result will be range mapped to this range after renormalization
	float	 m_scale = 1.f;
	int		 m_numOctaves = 1;
	float	 m_octavePersistence = 0.5f;
	float	 m_octaveScale = 2.f;
	bool	 m_renormalize = true;
	int		 m_seedOffset = 0;
};