// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/EngineConstantBuffers.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
namespace RenderConstants
{
    // Higher depth = deeper into screen = background
	// Lower depth = closer to camera = foreground
    constexpr float s_minSpriteRenderDepth      = 0.1f;
    constexpr float s_defaultSpriteRenderDepth  = 0.5f;
    constexpr float s_maxSpriteRenderDepth      = 0.9f;
	constexpr float s_invalidSpriteRenderDepth  = -1.f;
	constexpr float s_maxExpectedSpriteHeight   = 10.f;
}



//----------------------------------------------------------------------------------------------------------------------
enum class RenderFlags : uint8_t
{
	RotateSprite    = 1 << 0, // if true, render the sprite rotated to orientation, if false, do not rotate
	IsInCameraView  = 1 << 1, // True if the entity is in camera view this frame, according to size and location.
	IsHidden        = 1 << 2, // If true, do not render at all
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
	bool GetIsInCameraView() const;
	bool GetIsHidden() const;

    void SetRotateSprite(bool rotate);
	void SetIsInCameraView(bool isInView);
	void SetIsHidden(bool isHidden);

public:
    
	uint8_t m_renderFlags           = 0;
    float m_depthOverride           = RenderConstants::s_invalidSpriteRenderDepth;
    Vec2 m_pos                      = Vec2::ZeroVector;
    Vec2 m_renderOffset			    = Vec2::ZeroVector;
    float m_orientation             = 0.f;
    float m_renderRadius            = 0.5f;
    Rgba8 m_tint                    = Rgba8::White;
    Rgba8 m_baseTint                = Rgba8::White;
};
