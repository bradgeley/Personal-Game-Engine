// Bradley Christensen - 2025
#pragma once



class Texture;



//----------------------------------------------------------------------------------------------------------------------
// Swapchain
//
class Swapchain
{
    friend class RendererInterface;

protected:

    Swapchain() = default;
    Swapchain(Swapchain const& copy) = delete;
    virtual ~Swapchain() = default;

public:

    virtual void ReleaseResources() = 0;
    virtual void Present() = 0;
};