// Bradley Christensen - 2025
#pragma once
#include "Engine/Math/IntVec2.h"
#include <cstdint>



class Texture;
class Swapchain;



//----------------------------------------------------------------------------------------------------------------------
typedef uint32_t RenderTargetID;
extern uint32_t INVALID_RENDER_TARGET_ID;



//----------------------------------------------------------------------------------------------------------------------
struct RenderTarget
{
    virtual ~RenderTarget() = default;
    virtual void ReleaseResources();

    virtual void Present();

    RenderTargetID m_id             = INVALID_RENDER_TARGET_ID;
    IntVec2 m_renderDimensions;
    Texture* m_backbufferTexture    = nullptr;
    Texture* m_depthBuffer          = nullptr;
    Swapchain* m_swapchain          = nullptr; // optional
};
