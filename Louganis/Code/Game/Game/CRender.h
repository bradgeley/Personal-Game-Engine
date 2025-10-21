// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/EngineConstantBuffers.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
enum class RenderFlags : uint8_t
{
	RotateSprite    = 1 << 0, // if true, render the sprite rotated to orientation, if false, do not rotate
	IsInCameraView  = 1 << 1, // True if the entity is in camera view this frame, according to size and location.
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

    void SetRotateSprite(bool rotate);
	void SetIsInCameraView(bool isInView);

public:
    
	uint8_t m_renderFlags           = 0;
    Vec2 m_pos                      = Vec2::ZeroVector;
    Vec2 m_renderOffset			    = Vec2::ZeroVector;
    float m_orientation             = 0.f;
    float m_renderRadius            = 1.f;
    Rgba8 m_tint                    = Rgba8::White;
};
