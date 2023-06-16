// Bradley Christensen - 2022-2023
#pragma once
#include "D3D11Utils.h"



//----------------------------------------------------------------------------------------------------------------------
struct WindowRenderContext
{
    Texture* m_backbufferTexture    = nullptr;
    Texture* m_depthBuffer          = nullptr;
    IDXGISwapChain* m_swapChain     = nullptr;
};
