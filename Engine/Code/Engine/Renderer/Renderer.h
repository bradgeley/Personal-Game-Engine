// Bradley Christensen - 2022
#pragma once
#include "Engine/Core/EngineSubsystem.h"



class Texture;
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;
struct ID3D11BlendState;
enum   D3D11_BLEND : int;
enum   D3D11_BLEND_OP : int;



extern class Renderer* g_renderer;



//----------------------------------------------------------------------------------------------------------------------
enum class BlendMode
{
    Opaque = 0,
    Alpha,
    Additive,

    Count
};



//----------------------------------------------------------------------------------------------------------------------
struct RendererConfig
{
    
};



//----------------------------------------------------------------------------------------------------------------------
// Renderer
//
// The game's renderer! Make's triangles show up and stuff.
//
class Renderer : public EngineSubsystem
{
public:

    Renderer(RendererConfig const& config);
    
    virtual void Startup() override;
    virtual void BeginFrame() override;
    
    void BindRenderTarget( Texture* backbuffer );
    void SetBlendMode( BlendMode blendMode );

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetContext() const;
    
    ID3D11BlendState* CreateBlendState(D3D11_BLEND srcFactor, D3D11_BLEND dstFactor, D3D11_BLEND_OP op);

private:

    void CreateRenderContext();
    void DestroyRenderContext();
    void CreateBlendStates();
    void DestroyBlendStates();

private:

    RendererConfig const m_config;

    Texture* m_backbufferTexture = nullptr;
    
    // D3D11 Things
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11BlendState* m_blendStateByMode[(size_t) BlendMode::Count] = {};
};
