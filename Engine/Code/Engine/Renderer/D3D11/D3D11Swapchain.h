// Bradley Christensen - 2025
#pragma once
#include "Engine/Renderer/Swapchain.h"



struct IDXGISwapChain;



//----------------------------------------------------------------------------------------------------------------------
// D3D11Swapchain
//
class D3D11Swapchain : public Swapchain
{
    friend class D3D11Renderer;
    friend class D3D11Texture;

protected:

    D3D11Swapchain() = default;
    D3D11Swapchain(D3D11Swapchain const& copy) = delete;
    virtual ~D3D11Swapchain() = default;
    
public:

    virtual void ReleaseResources() override;
    virtual void Present() override;

protected:

    IDXGISwapChain* m_swapChain = nullptr;
};