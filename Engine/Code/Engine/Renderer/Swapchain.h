// Bradley Christensen - 2022-2025
#pragma once



class Texture;



//----------------------------------------------------------------------------------------------------------------------
// Swapchain
//
class Swapchain
{
    friend class Renderer;

protected:

    Swapchain() = default;
    Swapchain(Swapchain const& copy) = delete;
    virtual ~Swapchain() = default;

public:

    virtual void ReleaseResources() = 0;
    virtual void Present() = 0;
};