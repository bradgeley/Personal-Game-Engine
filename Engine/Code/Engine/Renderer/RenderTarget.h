// Bradley Christensen - 2025
#pragma once
#include "Engine/Math/IntVec2.h"



class Texture;



//----------------------------------------------------------------------------------------------------------------------
struct RenderTarget
{
    virtual ~RenderTarget() = default;

    IntVec2 m_renderDimensions;
    Texture* m_backbufferTexture    = nullptr;
    Texture* m_depthBuffer          = nullptr;
};
