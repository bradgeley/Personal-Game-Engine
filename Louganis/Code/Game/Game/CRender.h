// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/EngineConstantBuffers.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
enum class RenderFlags : uint8_t
{
	RotateSprite = 1 << 0, // if true, render the sprite rotated to orientation, if false, do not rotate
};



//----------------------------------------------------------------------------------------------------------------------
struct CRender
{
public:

    CRender() = default;
    CRender(void const* xmlElement);

    Vec2 GetRenderPosition() const;
    float GetRenderOrientation() const;

    bool GetRotateSprite() const;

    void SetRotateSprite(bool rotate);

public:
    
	uint8_t m_renderFlags           = 0;
    Vec2 m_pos                      = Vec2::ZeroVector;
    Vec2 m_renderOffset			    = Vec2::ZeroVector;
    float m_orientation             = 0.f;
    float m_scale                   = 1.f;
    Rgba8 m_tint                    = Rgba8::White;
};
