// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Math/Vec2.h"
#include <string>



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_SMALL_FLOATING_TEXT_LENGTH = 8;



//----------------------------------------------------------------------------------------------------------------------
struct FloatingTextInstance
{
	std::string m_text;
	Vec2 m_pos;
	Vec2 m_velocity;
	float m_scale			= 1.f;
	float m_lifetimeSeconds	= 1.f;
	float m_elapsedSeconds	= 0.f;
	Rgba8 m_tint			= Rgba8::White;
};