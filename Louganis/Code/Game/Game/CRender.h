// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/EngineConstantBuffers.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CRender
{
    CRender() = default;
    CRender(void const* xmlElement);

	Vec2 GetRenderPosition() const { return m_pos + (m_renderOffset * m_scale * 0.5f); }
    
    Vec2 m_pos                      = Vec2::ZeroVector;
    Vec2 m_renderOffset			    = Vec2::ZeroVector;
    float m_orientation             = 0.f;
    float m_scale                   = 1.f;
    Rgba8 m_tint                    = Rgba8::White;
};
