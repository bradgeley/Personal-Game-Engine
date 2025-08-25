// Bradley Christensen - 2025
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/Renderer/RendererUtils.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct RenderTarget
{
    virtual ~RenderTarget() = default;
    virtual void ReleaseResources();

    virtual void Present();

    IntVec2 m_renderDimensions;
    TextureID m_backbufferTexture    = RendererUtils::InvalidID;
    TextureID m_depthBuffer          = RendererUtils::InvalidID;
    SwapchainID m_swapchain          = RendererUtils::InvalidID;
};
