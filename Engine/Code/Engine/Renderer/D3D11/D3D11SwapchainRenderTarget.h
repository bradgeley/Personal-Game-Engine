// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Renderer/RenderTarget.h"



struct IDXGISwapChain;



//----------------------------------------------------------------------------------------------------------------------
struct D3D11SwapchainRenderTarget : public RenderTarget
{
	IDXGISwapChain* m_swapChain; // todo: abstract Swapchain out
};
