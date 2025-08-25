// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Renderer/Texture.h"



struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct IDXGISwapChain;



//----------------------------------------------------------------------------------------------------------------------
// D3D11 Texture
//
class D3D11Texture : public Texture
{
    friend class D3D11Renderer;

public:

    virtual void ReleaseResources();

    virtual bool IsValid() const;

    virtual bool CreateFromImage(Image const& image, bool createMipMap = true) override;

    // Renderer constructor for creating a texture from the swap chain backbuffer
    bool InitAsBackbufferTexture(IDXGISwapChain* swapChain);
    bool InitAsDepthBuffer(IDXGISwapChain* swapChain);

    ID3D11DepthStencilView* CreateOrGetDepthStencilView();
    ID3D11RenderTargetView* CreateOrGetRenderTargetView();
    ID3D11ShaderResourceView* CreateOrGetShaderResourceView();

protected:

    ID3D11Texture2D* m_textureHandle = nullptr;
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
    ID3D11ShaderResourceView* m_shaderResourceView = nullptr;
};

